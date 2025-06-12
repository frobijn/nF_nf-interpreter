//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//----------------------------------------------------------------------
//
// Global settings
//
//----------------------------------------------------------------------
#define MEMORY_ALLOCATION                        MALLOC_CAP_8BIT
#define DEFAULT_INTERRUPT_QUEUE_SIZE             16
#define DEFAULT_TASK_STACK_SIZE                  4096
#define DEFAULT_TASK_AFTER_INTERRUPT_PRIORITY    13
#define DEFAULT_TASK_MANAGED_ACTIVATION_PRIORITY 10

//----------------------------------------------------------------------
//
// Diagnostics
//
//----------------------------------------------------------------------
// Create a CLR version with diagnostics enabled to get additional
// information about the execution of the target-specific code.
// Use the CLR version with the NanoCLR.ISR.Core.Tests unit tests
// to ensure, e.g., that the memory allocation can be done.
// stack sizes are sufficient, etc.
//----------------------------------------------------------------------

#define ENABLE_ISR_MEMORY_DIAGNOSTICS
#define ENABLE_ISR_TASK_DIAGNOSTICS

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
static void ISR_Diagnostics_Memory(const char *method, InterpreterMemoryType memoryType)
{
    int realStringSize;
    char temporaryStringBuffer[100];
    size_t ramFree = heap_caps_get_free_size(MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL);
    size_t ramBlock = heap_caps_get_largest_free_block(MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL);
    if (memoryType == InterpreterMemoryType::InterpreterMemoryType_ISR)
    {
        realStringSize = snprintf(
            temporaryStringBuffer,
            sizeof(temporaryStringBuffer),
            "%s(%d): D/IRAM free/block %d / %d\r\n",
            method,
            (CLR_INT32)memoryType,
            ramFree,
            ramBlock);
    }
    else
    {
        size_t spiRamFree = heap_caps_get_free_size(MEMORY_ALLOCATION | MALLOC_CAP_SPIRAM);
        size_t spiRamBlock = heap_caps_get_largest_free_block(MEMORY_ALLOCATION | MALLOC_CAP_SPIRAM);
        realStringSize = snprintf(
            temporaryStringBuffer,
            sizeof(temporaryStringBuffer),
            "%s(%d): D/IRAM free/block %d / %d, SPIRAM %d / %d\r\n",
            method,
            (CLR_INT32)memoryType,
            ramFree,
            ramBlock,
            spiRamFree,
            spiRamBlock);
    }
    CLR_EE_DBG_EVENT_BROADCAST(
        CLR_DBG_Commands_c_Monitor_Message,
        realStringSize,
        temporaryStringBuffer,
        WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
}
#endif // ENABLE_ISR_MEMORY_DIAGNOSTICS

#ifdef ENABLE_ISR_TASK_DIAGNOSTICS

#define ISR_DIAGNOSTICS_STACK_SIZE 1500

static void ISR_Diagnostics_StackSize(const char *message, CLR_UINT32 usedStackSize)
{
    char temporaryStringBuffer[100];
    int realStringSize = snprintf(
        temporaryStringBuffer,
        sizeof(temporaryStringBuffer),
        "Used stack size (%s): %d\r\n",
        message,
        usedStackSize);
    CLR_EE_DBG_EVENT_BROADCAST(
        CLR_DBG_Commands_c_Monitor_Message,
        realStringSize,
        temporaryStringBuffer,
        WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
}

#else

#define ISR_DIAGNOSTICS_STACK_SIZE 0

#endif // ENABLE_ISR_TASK_DIAGNOSTICS

//======================================================================
//
// Configuration and state
//
//======================================================================
struct RTOSTaskData;
struct ManagedActivationTaskData;

typedef struct __nfpack NF_Runtime_ISR_Esp32_Configuration
{
    NF_Runtime_ISR_Configuration Shared;
    struct RTOSTaskData *AfterInterruptTask;
    struct ManagedActivationTaskData *ManagedActivationTask;
} NF_Runtime_ISR_Esp32_Configuration;

NF_Runtime_ISR_MemoryOffsetType NF_Runtime_ISR_GetConfigurationMemorySize()
{
    return sizeof(NF_Runtime_ISR_Esp32_Configuration);
}

//======================================================================
//
// Memory allocation.
//
//======================================================================

static void *AllocateMemory(
    InterpreterMemoryType memoryType,
    NF_Runtime_ISR_MemoryOffsetType size,
    bool isISRMemorySpecial)
{
    void *memory;
    if (memoryType == InterpreterMemoryType::InterpreterMemoryType_ISR && isISRMemorySpecial)
    {
        memory = heap_caps_malloc(size, MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL);
    }
    else
    {
        memory = heap_caps_malloc_prefer(
            size,
            2,
            MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL,
            MEMORY_ALLOCATION | MALLOC_CAP_SPIRAM);
    }

    if (memory != nullptr)
    {
        memset(memory, 0, size);
    }

    return memory;
}

HRESULT NF_Runtime_ISR_AllocateMemory(
    InterpreterMemoryType memoryType,
    NF_Runtime_ISR_MemoryOffsetType size,
    bool isISRMemorySpecial,
    void **memory)
{
    NANOCLR_HEADER();

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("Before AllocateMemory", memoryType);
#endif
    *memory = AllocateMemory(memoryType, size, isISRMemorySpecial);

    if (*memory == nullptr)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
    }

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After AllocateMemory", memoryType);
#endif
    NANOCLR_NOCLEANUP();
}

HRESULT NF_Runtime_ISR_ReleaseMemory(
    NF_Runtime_ISR_Configuration *configuration,
    InterpreterMemoryType memoryType,
    void *memory)
{
    NANOCLR_HEADER();

    if (memory != nullptr)
    {
        heap_caps_free(memory);
    }

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After ReleaseMemory", memoryType);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

//======================================================================
//
// Locks
//
//======================================================================

NF_Runtime_ISR_MemoryOffsetType NF_Runtime_ISR_LockGetMemorySize()
{
    return sizeof(SemaphoreHandle_t);
}

HRESULT NF_Runtime_ISR_LockInitialise(void *mutex)
{
    NANOCLR_HEADER();

    *(SemaphoreHandle_t *)mutex = xSemaphoreCreateMutex();

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT NF_Runtime_ISR_LockRelease(void *mutex)
{
    NANOCLR_HEADER();

    vSemaphoreDelete(*(SemaphoreHandle_t *)mutex);

    NANOCLR_NOCLEANUP_NOLABEL();
}

void NF_Runtime_ISR_LockEnterScope(void *mutex)
{
    while (true)
    {
        if (xSemaphoreTake(*(SemaphoreHandle_t *)mutex, portMAX_DELAY))
        {
            return;
        }
    }
}

void NF_Runtime_ISR_LockExitScope(void *mutex)
{
    xSemaphoreGive(*(SemaphoreHandle_t *)mutex);
}

//======================================================================
//
// Sleep
//
//======================================================================

void NF_Runtime_ISR_Sleep(CLR_UINT32 time_us)
{
    TickType_t delay = pdMS_TO_TICKS(time_us) / (TickType_t)1000U;
    vTaskDelay(delay == 0 ? 1 : delay);
}

//======================================================================
//
// RTOS task for AfterInterrupt service routines.
//
//======================================================================

typedef struct RTOSTaskData
{
    TaskHandle_t Task;
    StaticTask_t TaskBuffer;
    QueueHandle_t Queue;
    StackType_t Stack[ISR_DIAGNOSTICS_STACK_SIZE];
} RTOSTaskData;

static void AfterInterruptTask(void *arg)
{
    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)arg;
    while (true)
    {
        NF_Runtime_ISR_InterruptHandler *interruptHandler;
        if (xQueueReceive(state->AfterInterruptTask->Queue, &interruptHandler, portMAX_DELAY) == pdTRUE)
        {
            if (state->Shared.MonitorResourceUse)
            {
                CLR_UINT32 required = 1 + uxQueueMessagesWaiting(state->AfterInterruptTask->Queue);
                if (state->Shared.RequiredInterruptQueueSize < required)
                {
                    state->Shared.RequiredInterruptQueueSize = required;
                }
            }

            NF_Runtime_ISR_RunFromRTOSTask(interruptHandler);
#ifdef ENABLE_ISR_TASK_DIAGNOSTICS
            CLR_UINT32 usedStackSize =
                state->Shared.AfterInterruptStackSize + ISR_DIAGNOSTICS_STACK_SIZE - uxTaskGetStackHighWaterMark2(NULL);
            ISR_Diagnostics_StackSize("RTOS task", usedStackSize);
            if (state->Shared.MonitorResourceUse && state->Shared.RequiredAfterInterruptStackSize < usedStackSize)
            {
                state->Shared.RequiredAfterInterruptStackSize = usedStackSize;
            }
#else
            if (state->Shared.MonitorResourceUse)
            {
                CLR_UINT32 usedStackSize = state->Shared.AfterInterruptStackSize + ISR_DIAGNOSTICS_STACK_SIZE -
                                           uxTaskGetStackHighWaterMark2(NULL);
                if (state->Shared.RequiredAfterInterruptStackSize < usedStackSize)
                {
                    state->Shared.RequiredAfterInterruptStackSize = usedStackSize;
                }
            }
#endif
        }
    }
}

HRESULT NF_Runtime_ISR_AllocateRTOSTaskData(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;
    if (state->AfterInterruptTask == nullptr)
    {
        if (configuration->AfterInterruptStackSize == 0)
        {
            configuration->AfterInterruptStackSize = DEFAULT_TASK_STACK_SIZE;
        }

        NF_Runtime_ISR_MemoryOffsetType size = sizeof(RTOSTaskData) + configuration->AfterInterruptStackSize;
        void *taskData =
            AllocateMemory(InterpreterMemoryType::InterpreterMemoryType_ISR, size, configuration->IsISRMemorySpecial);
        if (taskData == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        state->AfterInterruptTask = (RTOSTaskData *)taskData;
#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
        ISR_Diagnostics_Memory("After AllocateRTOSTaskData", InterpreterMemoryType_ISR);
#endif
    }

    NANOCLR_NOCLEANUP();
}

HRESULT NF_Runtime_ISR_ReleaseRTOSTaskData(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;
    if (state->AfterInterruptTask != nullptr)
    {
        heap_caps_free(state->AfterInterruptTask);
        state->AfterInterruptTask = nullptr;
    }

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After ReleaseRTOSTaskData", InterpreterMemoryType_ISR);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT NF_Runtime_ISR_EnableRTOSTask(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;

    if (state->Shared.InterruptQueueSize == 0)
    {
        state->Shared.InterruptQueueSize = DEFAULT_INTERRUPT_QUEUE_SIZE;
    }

    if (state->Shared.AfterInterruptPriority == 0)
    {
        state->Shared.AfterInterruptPriority = DEFAULT_TASK_AFTER_INTERRUPT_PRIORITY;
    }

    state->AfterInterruptTask->Queue =
        xQueueCreate(state->Shared.InterruptQueueSize, sizeof(NF_Runtime_ISR_InterruptHandler *));
    if (!state->AfterInterruptTask->Queue)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    state->AfterInterruptTask->Task = xTaskCreateStatic(
        AfterInterruptTask,
        "ISR-RTOS-Task",
        state->Shared.AfterInterruptStackSize + ISR_DIAGNOSTICS_STACK_SIZE,
        state,
        state->Shared.AfterInterruptPriority,
        state->AfterInterruptTask->Stack,
        &state->AfterInterruptTask->TaskBuffer);
    if (!state->AfterInterruptTask->Task)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT NF_Runtime_ISR_DisableRTOSTask(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;
    if (state->AfterInterruptTask->Task)
    {
        vTaskSuspend(state->AfterInterruptTask->Task);
        vTaskDelete(state->AfterInterruptTask->Task);
        state->AfterInterruptTask->Task = 0;
    }

    if (state->AfterInterruptTask->Queue)
    {
        vQueueDelete(state->AfterInterruptTask->Queue);
        state->AfterInterruptTask->Queue = 0;
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

void NF_Runtime_ISR_QueueRTOSTask(NF_Runtime_ISR_InterruptHandler *interruptHandler)
{
    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)interruptHandler->Configuration;
    if (state->AfterInterruptTask->Queue)
    {
        BaseType_t xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(state->AfterInterruptTask->Queue, &interruptHandler, &xHigherPriorityTaskWoken);
        if (xHigherPriorityTaskWoken)
        {
            portYIELD_FROM_ISR();
        }
    }
}

//======================================================================
//
// RTOS task for service routine activated from managed code.
//
//======================================================================

typedef struct ManagedActivationTaskData
{
    TaskHandle_t Task;
    StaticTask_t TaskBuffer;
    NF_Runtime_ISR_ManagedActivation ServiceRoutine;
    StackType_t Stack[ISR_DIAGNOSTICS_STACK_SIZE];
} ManagedActivationTaskData;

static void ManagedActivatedTask(void *arg)
{
    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)arg;
    NF_Runtime_ISR_RunServiceRoutine(&state->ManagedActivationTask->ServiceRoutine);

#ifdef ENABLE_ISR_TASK_DIAGNOSTICS
    CLR_UINT32 usedStackSize =
        state->Shared.ManagedTaskStackSize + ISR_DIAGNOSTICS_STACK_SIZE - uxTaskGetStackHighWaterMark2(NULL);
    ISR_Diagnostics_StackSize("Managed activation task", usedStackSize);
    if (state->Shared.MonitorResourceUse && state->Shared.RequiredManagedTaskStackSize < usedStackSize)
    {
        state->Shared.RequiredManagedTaskStackSize = usedStackSize;
    }
#else
    if (state->Shared.MonitorResourceUse)
    {
        CLR_UINT32 usedStackSize =
            state->Shared.ManagedTaskStackSize + ISR_DIAGNOSTICS_STACK_SIZE - uxTaskGetStackHighWaterMark2(NULL);
        if (state->Shared.RequiredManagedTaskStackSize < usedStackSize)
        {
            state->Shared.RequiredManagedTaskStackSize = usedStackSize;
        }
    }
#endif

    state->ManagedActivationTask->Task = 0;
    vTaskDelete(NULL);
}

HRESULT NF_Runtime_ISR_AllocateManagedActivationTaskData(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;
    if (state->ManagedActivationTask == nullptr)
    {
        if (configuration->ManagedTaskStackSize == 0)
        {
            configuration->ManagedTaskStackSize = DEFAULT_TASK_STACK_SIZE;
        }

        NF_Runtime_ISR_MemoryOffsetType size = sizeof(ManagedActivationTaskData) + configuration->ManagedTaskStackSize;
        void *taskData = AllocateMemory(
            InterpreterMemoryType::InterpreterMemoryType_ManagedActivation,
            size,
            configuration->IsISRMemorySpecial);
        if (taskData == nullptr)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OUT_OF_MEMORY);
        }

        state->ManagedActivationTask = (ManagedActivationTaskData *)taskData;
#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
        ISR_Diagnostics_Memory("After AllocateMATaskData", InterpreterMemoryType_ISR);
#endif
    }
    NANOCLR_NOCLEANUP();
}

HRESULT NF_Runtime_ISR_ReleaseManagedActivationTaskData(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;
    if (state->ManagedActivationTask != nullptr)
    {
        heap_caps_free(state->ManagedActivationTask);
        state->ManagedActivationTask = nullptr;
    }

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After ReleaseMATaskData", InterpreterMemoryType_ISR);
#endif

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT NF_Runtime_ISR_StartManagedActivationTask(
    NF_Runtime_ISR_Configuration *configuration,
    NF_Runtime_ISR_ManagedActivation *serviceRoutine)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;
    if (state->Shared.ManagedTaskPriority == 0)
    {
        state->Shared.ManagedTaskPriority = DEFAULT_TASK_MANAGED_ACTIVATION_PRIORITY;
    }

    state->ManagedActivationTask->ServiceRoutine = *serviceRoutine;
    state->ManagedActivationTask->Task = xTaskCreateStatic(
        ManagedActivatedTask,
        "ISR-Activated-Task",
        state->Shared.ManagedTaskStackSize + ISR_DIAGNOSTICS_STACK_SIZE,
        state,
        state->Shared.ManagedTaskPriority,
        state->ManagedActivationTask->Stack,
        &state->ManagedActivationTask->TaskBuffer);

    if (!state->ManagedActivationTask->Task)
    {
        NANOCLR_SET_AND_LEAVE(CLR_E_FAIL);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT NF_Runtime_ISR_StopManagedActivationTask(NF_Runtime_ISR_Configuration *configuration)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_Esp32_Configuration *state = (NF_Runtime_ISR_Esp32_Configuration *)configuration;

    if (state->ManagedActivationTask != nullptr && !state->ManagedActivationTask->Task)
    {
        vTaskSuspend(state->ManagedActivationTask->Task);
        vTaskDelete(state->ManagedActivationTask->Task);
        state->ManagedActivationTask->Task = 0;
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}
