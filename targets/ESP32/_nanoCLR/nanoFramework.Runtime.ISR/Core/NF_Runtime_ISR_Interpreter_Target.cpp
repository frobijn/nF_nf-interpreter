//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//======================================================================
//
// Memory allocation.
// 
//======================================================================

#define MEMORY_ALLOCATION  MALLOC_CAP_8BIT | MALLOC_CAP_32BIT

void *NF_RunTime_ISR_AllocateMemory (InterpreterMemoryType memoryType, NF_Runtime_ISR_SharedDataOffsetType size)
{
    if (InterpreterMemoryType::InterpreterMemoryType_ISR)
    {
        return heap_caps_malloc (size, MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL);
    }
    else
    {
        return heap_caps_malloc_prefer (size, 2, MEMORY_ALLOCATION | MALLOC_CAP_INTERNAL, MEMORY_ALLOCATION | MALLOC_CAP_SPIRAM);
    }
}

void NF_RunTime_ISR_ReleaseMemory (InterpreterMemoryType memoryType, void *memory)
{
    heap_caps_free (memory);
}

//======================================================================
//
// RTOS task for AfterInterrupt service routines.
// 
//======================================================================

// The stack size for the RTOS task. This is a wild guess for now.
#define TASK_STACK_SIZE 1024
#define TASK_PRIORITY 10

static const char *TAG = "ISR";

typedef struct RTOSTaskData
{
    TaskHandle_t Task;
    StaticTask_t TaskBuffer;
    QueueHandle_t Queue;
    StackType_t Stack[TASK_STACK_SIZE];
} RTOSTaskData;

static void AfterInterruptTask (void *arg)
{
    RTOSTaskData *data = (RTOSTaskData *)arg;

    while (true)
    {
        NF_Runtime_ISR_InterruptHandler *interruptHandler;
        if (xQueueReceive(data->Queue, &interruptHandler, 10) == pdTRUE) 
        {
            NF_RunTime_ISR_RunFromRTOSTask(interruptHandler);
        }
    }
}

NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_GetRTOSTaskMemorySize ()
{
    return sizeof (RTOSTaskData);
}

void NF_RunTime_ISR_EnableRTOSTask (void *taskData, NF_Runtime_ISR_HeapOffsetType queueSize)
{
    RTOSTaskData *data = (RTOSTaskData *)taskData;

    data->Queue = xQueueCreate(queueSize, sizeof(NF_Runtime_ISR_InterruptHandler *));
    if (!data->Queue)
    {
        ESP_LOGE(TAG, "Failed to create queue for RTOS task");
        return;
    }

    data->Task = xTaskCreateStatic(AfterInterruptTask, "ISR-RTOS-Task", TASK_STACK_SIZE, taskData, TASK_PRIORITY, data->Stack, &data->TaskBuffer);
    if (!data->Task)
    {
        ESP_LOGE(TAG, "Failed to create RTOS task");
        return;
    }
}

void NF_RunTime_ISR_DisableRTOSTask (void *taskData)
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

void NF_RunTime_ISR_QueueRTOSTask (NF_Runtime_ISR_InterruptHandler *interruptHandler, void *taskData)
{
    RTOSTaskData *data = (RTOSTaskData *)taskData;
    if (data->Queue)
    {
        BaseType_t xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(data->Queue, &interruptHandler, &xHigherPriorityTaskWoken);
        if( xHigherPriorityTaskWoken )
        {
            portYIELD_FROM_ISR ();
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
    StaticTask_t TaskBuffer;
    StackType_t Stack[TASK_STACK_SIZE];
    NF_Runtime_ISR_ManagedActivation ServiceRoutine;
} ManagedActivationTaskData;

static void ManagedActivatedTask (void *arg)
{
    ManagedActivationTaskData *data = (ManagedActivationTaskData *)arg;
    NF_RunTime_ISR_RunServiceRoutine (&data->ServiceRoutine);
    vTaskDelete(NULL);
}

void NF_RunTime_ISR_RunServiceRoutineInSeparateRTOSTask (NF_Runtime_ISR_ManagedActivation *serviceRoutine)
{
    ManagedActivationTaskData taskData;
    taskData.ServiceRoutine = *serviceRoutine;
 
    TaskHandle_t task;
    task = xTaskCreateStatic(ManagedActivatedTask, "ISR-Activated-Task", TASK_STACK_SIZE, &taskData, TASK_PRIORITY, taskData.Stack, &taskData.TaskBuffer);
    if (!task)
    {
        ESP_LOGE(TAG, "Failed to create managed activated task");
        return;
    }
}



