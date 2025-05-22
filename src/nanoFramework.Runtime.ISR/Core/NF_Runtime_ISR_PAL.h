//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//======================================================================
//
// This header for the platform specific functionality of the ISR
// interpreter is shared by the nf-interpreter and the
// nanoFramework.Runtime.ISR.Interpreter project. If this file is
// modified in one git repository, make sure to copy it to to the
// other repository as well.
//
//======================================================================

#ifndef NF_RUNTIME_ISR_PAL_H
#define NF_RUNTIME_ISR_PAL_H

#include "NF_Runtime_ISR.h"

#ifndef NF_RUNTIME_ISR_UNITTESTS
/// <summary>
/// Allocate a block of memory.
/// </summary>
/// <param name="memoryType">Type of memory to allocate.</param>
/// <param name="size">Size of the memory to allocate.</param>
/// <returns>The pointer to the memory, or the null pointer if the memory cannot be allocated.</returns>
extern void *NF_RunTime_ISR_AllocateMemory(InterpreterMemoryType memoryType, NF_Runtime_ISR_MemoryOffsetType size);

/// <summary>
/// Release a block of memory
/// </summary>
/// <param name="memoryType">Type of memory to allocate.</param>
/// <param name="memory">The allocated memory.</param>
/// <returns>The pointer to the memory, or the null pointer if the memory cannot be allocated.</returns>
extern void NF_RunTime_ISR_ReleaseMemory(InterpreterMemoryType memoryType, void *memory);

/// <summary>
/// Allocate the memory required to run an RTOS task that runs the service routine
/// that is started from managed code.
/// </summary>
extern void *NF_RunTime_ISR_AllocateManagedActivationTaskData();

/// <summary>
/// Release the memory required to run an RTOS task that runs the service routine
/// that is started from managed code.
/// </summary>
/// <param name="taskData">The memory returned by
/// <see cref="NF_RunTime_ISR_AllocateManagedActivationTaskData"/>.</param>
extern void NF_RunTime_ISR_ReleaseManagedActivationTaskData(void *taskData);

/// <summary>
/// Create a RTOS task to run the service routine.
/// Call the <see cref="NF_RunTime_ISR_RunServiceRoutine"/> method from the task.
/// </summary>
/// <param name="taskData">The memory returned by
/// <see cref="NF_RunTime_ISR_AllocateManagedActivationTaskData"/>.</param>
/// <param name="serviceRoutine">Description of the service routine to call.</param>
extern void NF_RunTime_ISR_StartManagedActivationTask(void *taskData, NF_Runtime_ISR_ManagedActivation *serviceRoutine);

/// <summary>
/// Stop the RTOS task started by <see cref="NF_RunTime_ISR_StartManagedActivationTask"/>.
/// </summary>
/// <param name="taskData">The memory returned by
/// <see cref="NF_RunTime_ISR_AllocateManagedActivationTaskData"/>.</param>
extern void NF_RunTime_ISR_StopManagedActivationTask(void *taskData);

/// <summary>
/// Allocate the memory required to run an RTOS task that calls service routines after
/// an interrupt has been handled.
/// </summary>
extern void *NF_RunTime_ISR_AllocateRTOSTaskData();

/// <summary>
/// Release the memory required to run an RTOS task that calls service routines after
/// an interrupt has been handled.
/// </summary>
/// <param name="taskData">The memory returned by <see cref="NF_RunTime_ISR_AllocateRTOSTaskData"/>.</param>
extern void NF_RunTime_ISR_ReleaseRTOSTaskData(void *taskData);

/// <summary>
/// Enable/start the RTOS task that calls service routines after an interrupt has been handled.
/// </summary>
/// <param name="taskData">The memory returned by <see cref="NF_RunTime_ISR_AllocateRTOSTaskData"/>.</param>
/// <param name="queueSize">The number of interrupts that can be queued for execution by the task.</param>
extern void NF_RunTime_ISR_EnableRTOSTask(void *taskData, NF_Runtime_ISR_MemoryOffsetType queueSize);
#endif

/// <summary>
/// Put the service routine(s) in a queue to be run in a RTOS task.
/// Call the <see cref="NF_RunTime_ISR_RunFromRTOSTask"/> method from the task.
/// </summary>
/// <param name="interruptHandler">Native data required by the native interpreter to run the service routines.</param>
extern void NF_RunTime_ISR_QueueRTOSTask(NF_Runtime_ISR_InterruptHandler *interruptHandler);

#ifndef NF_RUNTIME_ISR_UNITTESTS
/// <summary>
/// Enable/start the RTOS task that calls service routines after an interrupt has been handled.
/// </summary>
/// <param name="taskData">The memory returned by <see cref="NF_RunTime_ISR_AllocateRTOSTaskData"/>.</param>
extern void NF_RunTime_ISR_DisableRTOSTask(void *taskData);
#endif

#endif // NF_RUNTIME_ISR_PAL_H
