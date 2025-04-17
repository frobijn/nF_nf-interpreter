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
#define MEMORY_ALLOCATION  MALLOC_CAP_8BIT | MALLOC_CAP_32BIT
#define TASK_STACK_SIZE    1024
#define TASK_PRIORITY      10
#define TASK_QUEUE_TIMEOUT 10

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
#define ENABLE_ISR_DIAGNOSTICS

#ifdef ENABLE_ISR_DIAGNOSTICS
static void ISR_Diagnostics(const char *message, CLR_UINT32 value)
{
    char temporaryStringBuffer[64];
    int realStringSize = snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "%s: %d\r\n", message, value);
    CLR_EE_DBG_EVENT_BROADCAST(
        CLR_DBG_Commands_c_Monitor_Message,
        realStringSize,
        temporaryStringBuffer,
        WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
}

static void ISR_Diagnostics_Memory(const char *method, InterpreterMemoryType memoryType)
{
    int realStringSize;
    char temporaryStringBuffer[80];
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

#define ISR_DIAGNOSTICS_STACK_SIZE 200

#else

#define ISR_DIAGNOSTICS_STACK_SIZE 0

#endif // ENABLE_ISR_DIAGNOSTICS

//======================================================================
//
// Memory allocation.
//
//======================================================================

void *NF_RunTime_ISR_AllocateMemory(InterpreterMemoryType memoryType, NF_Runtime_ISR_SharedDataOffsetType size)
{
#ifdef ENABLE_ISR_DIAGNOSTICS
    ISR_Diagnostics_Memory("AllocateMemory", memoryType);
#endif
    if (InterpreterMemoryType::InterpreterMemoryType_ISR)
    {
        return heap_caps_malloc(size, MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL);
    }
    else
    {
        return heap_caps_malloc_prefer(
            size,
            2,
            MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL,
            MEMORY_ALLOCATION | MALLOC_CAP_SPIRAM);
    }
}

void NF_RunTime_ISR_ReleaseMemory(InterpreterMemoryType memoryType, void *memory)
{
    heap_caps_free(memory);

#ifdef ENABLE_ISR_DIAGNOSTICS
    ISR_Diagnostics_Memory("ReleaseMemory", memoryType);
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

#ifdef ENABLE_ISR_DIAGNOSTICS
    {
        CLR_UINT32 highWaterMark = uxTaskGetStackHighWaterMark2(NULL);
        ISR_Diagnostics("Used stack size", TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE - 4 * highWaterMark);
    }
#endif

    while (true)
    {
        NF_Runtime_ISR_InterruptHandler *interruptHandler;
        if (xQueueReceive(data->Queue, &interruptHandler, TASK_QUEUE_TIMEOUT) == pdTRUE)
        {
            NF_RunTime_ISR_RunFromRTOSTask(interruptHandler);
#ifdef ENABLE_ISR_DIAGNOSTICS
            CLR_UINT32 highWaterMark = uxTaskGetStackHighWaterMark2(NULL);
            ISR_Diagnostics("Used stack size", TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE - 4 * highWaterMark);
#endif
        }
    }
}

NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_GetRTOSTaskMemorySize()
{
    return sizeof(RTOSTaskData);
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
        TASK_PRIORITY,
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
        vTaskDelete(data->Task);
    }

    if (data->Queue)
    {
        vQueueDelete(data->Queue);
    }
}

void NF_RunTime_ISR_QueueRTOSTask(NF_Runtime_ISR_InterruptHandler *interruptHandler, void *taskData)
{
    RTOSTaskData *data = (RTOSTaskData *)taskData;
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

#ifdef ENABLE_ISR_DIAGNOSTICS
    CLR_UINT32 highWaterMark = uxTaskGetStackHighWaterMark2(NULL);
    ISR_Diagnostics("Used stack size", TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE - 4 * highWaterMark);
#endif

    vTaskDelete(NULL);
}

void NF_RunTime_ISR_RunServiceRoutineInSeparateRTOSTask(NF_Runtime_ISR_ManagedActivation *serviceRoutine)
{
    ManagedActivationTaskData taskData;
    taskData.ServiceRoutine = *serviceRoutine;

    TaskHandle_t task = xTaskCreateStatic(
        ManagedActivatedTask,
        "ISR-Activated-Task",
        TASK_STACK_SIZE + ISR_DIAGNOSTICS_STACK_SIZE,
        &taskData,
        TASK_PRIORITY,
        taskData.Stack,
        &taskData.TaskBuffer);

    if (!task)
    {
        ESP_LOGE(TAG, "Failed to create managed activated task");
        return;
    }
}
