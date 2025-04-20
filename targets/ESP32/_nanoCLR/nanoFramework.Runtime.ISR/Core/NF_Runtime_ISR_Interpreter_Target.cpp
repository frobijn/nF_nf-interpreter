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
#define MEMORY_ALLOCATION                MALLOC_CAP_8BIT
#define TASK_STACK_SIZE                  20240
#define TASK_AFTER_INTERRUPT_PRIORITY    13
#define TASK_MANAGED_ACTIVATION_PRIORITY 10
#define TASK_QUEUE_TIMEOUT               10

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

static void ISR_Diagnostics_StackSize(const char *message, CLR_UINT32 highWaterMark)
{
    char temporaryStringBuffer[100];
    int realStringSize = snprintf(
        temporaryStringBuffer,
        sizeof(temporaryStringBuffer),
        "Used stack size (%s): %d\r\n",
        message,
        TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE - highWaterMark);
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
// Memory allocation.
//
//======================================================================

void *NF_RunTime_ISR_AllocateMemory(InterpreterMemoryType memoryType, NF_Runtime_ISR_SharedDataOffsetType size)
{
#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("Before AllocateMemory", memoryType);
#endif
    void *memory;
    if (InterpreterMemoryType::InterpreterMemoryType_ISR)
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

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After AllocateMemory", memoryType);
#endif
    return memory;
}

void NF_RunTime_ISR_ReleaseMemory(InterpreterMemoryType memoryType, void *memory)
{
    heap_caps_free(memory);

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After ReleaseMemory", memoryType);
#endif
}

//======================================================================
//
// RTOS task for AfterInterrupt service routines.
//
//======================================================================

static const char *TAG = "ISR";

typedef struct RTOSTaskData
{
    TaskHandle_t Task;
    StaticTask_t TaskBuffer;
    QueueHandle_t Queue;
    StackType_t Stack[TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE];
} RTOSTaskData;

static void AfterInterruptTask(void *arg)
{
    RTOSTaskData *data = (RTOSTaskData *)arg;
    while (true)
    {
        NF_Runtime_ISR_InterruptHandler *interruptHandler;
        if (xQueueReceive(data->Queue, &interruptHandler, TASK_QUEUE_TIMEOUT) == pdTRUE)
        {
            NF_RunTime_ISR_RunFromRTOSTask(interruptHandler);
#ifdef ENABLE_ISR_TASK_DIAGNOSTICS
            ISR_Diagnostics_StackSize("RTOS task", uxTaskGetStackHighWaterMark2(NULL));
#endif
        }
    }
}

void *NF_RunTime_ISR_AllocateRTOSTaskData()
{
    void *taskData = heap_caps_malloc(sizeof(RTOSTaskData), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    if (taskData == nullptr)
    {
        ESP_LOGE(TAG, "Failed to allocate RTOS task data");
    }

    memset(taskData, 0, sizeof(RTOSTaskData));
#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After AllocateRTOSTaskData", InterpreterMemoryType_ISR);
#endif
    return taskData;
}

void NF_RunTime_ISR_ReleaseRTOSTaskData(void *taskData)
{
    heap_caps_free(taskData);

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After ReleaseRTOSTaskData", InterpreterMemoryType_ISR);
#endif
}

void NF_RunTime_ISR_EnableRTOSTask(void *taskData, NF_Runtime_ISR_HeapOffsetType queueSize)
{
    RTOSTaskData *data = (RTOSTaskData *)taskData;

    data->Queue = xQueueCreate(queueSize, sizeof(NF_Runtime_ISR_InterruptHandler *));
    if (!data->Queue)
    {
        ESP_LOGE(TAG, "Failed to create queue for RTOS task");
        return;
    }

    data->Task = xTaskCreateStatic(
        AfterInterruptTask,
        "ISR-RTOS-Task",
        TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE,
        taskData,
        TASK_AFTER_INTERRUPT_PRIORITY,
        data->Stack,
        &data->TaskBuffer);
    if (!data->Task)
    {
        ESP_LOGE(TAG, "Failed to create RTOS task");
        return;
    }
}

void NF_RunTime_ISR_DisableRTOSTask(void *taskData)
{
    RTOSTaskData *data = (RTOSTaskData *)taskData;
    if (data->Queue)
    {
        xQueueReset(data->Queue);
    }

    if (data->Task)
    {
        vTaskSuspend(data->Task);
        vTaskDelete(data->Task);
        data->Task = 0;
    }

    if (data->Queue)
    {
        vQueueDelete(data->Queue);
        data->Queue = 0;
    }
}

void NF_RunTime_ISR_QueueRTOSTask(NF_Runtime_ISR_InterruptHandler *interruptHandler)
{
    RTOSTaskData *data = (RTOSTaskData *)interruptHandler->TaskMemory;
    if (data->Queue)
    {
        BaseType_t xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(data->Queue, &interruptHandler, &xHigherPriorityTaskWoken);
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
    StackType_t Stack[TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE];
    NF_Runtime_ISR_ManagedActivation ServiceRoutine;
} ManagedActivationTaskData;

static void ManagedActivatedTask(void *arg)
{
    ManagedActivationTaskData *data = (ManagedActivationTaskData *)arg;
    NF_RunTime_ISR_RunServiceRoutine(&data->ServiceRoutine);

#ifdef ENABLE_ISR_TASK_DIAGNOSTICS
    ISR_Diagnostics_StackSize("Managed activation task", uxTaskGetStackHighWaterMark2(NULL));
#endif
    data->Task = 0;
    vTaskDelete(NULL);
}

void *NF_RunTime_ISR_AllocateManagedActivationTaskData()
{
    void *taskData = heap_caps_malloc(sizeof(ManagedActivationTaskData), MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
    if (taskData == nullptr)
    {
        ESP_LOGE(TAG, "Failed to allocate managed activated task data");
    }

    memset(taskData, 0, sizeof(ManagedActivationTaskData));
#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After AllocateMATaskData", InterpreterMemoryType_ISR);
#endif
    return taskData;
}

void NF_RunTime_ISR_ReleaseManagedActivationTaskData(void *taskData)
{
    heap_caps_free(taskData);

#ifdef ENABLE_ISR_MEMORY_DIAGNOSTICS
    ISR_Diagnostics_Memory("After ReleaseMATaskData", InterpreterMemoryType_ISR);
#endif
}

void NF_RunTime_ISR_StartManagedActivationTask(void *taskData, NF_Runtime_ISR_ManagedActivation *serviceRoutine)
{
    ManagedActivationTaskData *data = (ManagedActivationTaskData *)taskData;
    data->ServiceRoutine = *serviceRoutine;
    data->Task = xTaskCreateStatic(
        ManagedActivatedTask,
        "ISR-Activated-Task",
        TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE,
        taskData,
        TASK_MANAGED_ACTIVATION_PRIORITY,
        data->Stack,
        &data->TaskBuffer);

    if (!data->Task)
    {
        ESP_LOGE(TAG, "Failed to create managed activated task");
        return;
    }
}

void NF_RunTime_ISR_StopManagedActivationTask(void *taskData)
{
    ManagedActivationTaskData *data = (ManagedActivationTaskData *)taskData;

    if (!data->Task)
    {
        vTaskSuspend(data->Task);
        vTaskDelete(data->Task);
        data->Task = 0;
    }
}
