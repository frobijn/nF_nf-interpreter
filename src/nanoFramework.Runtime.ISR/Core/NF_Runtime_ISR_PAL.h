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
/// Get the size of the memory required to hold a (target specific version) of NF_Runtime_ISR_Configuration.
/// </summary>
extern NF_Runtime_ISR_MemoryOffsetType NF_Runtime_ISR_GetConfigurationMemorySize();

/// <summary>
/// Allocate a block of memory and set its content to all zeros.
/// </summary>
/// <param name="configuration">Device/platform configuration and (native) state for the service routine
/// manager.</param>
/// <param name="memoryType">Type of memory to allocate.</param>
/// <param name="size">Size of the memory to allocate.</param>
/// <param name="IsISRMemorySpecial">Indicates whether the memory to use in interrupt handlers is
/// special.</param>
/// <param name="memory">The pointer to the memory, or the null pointer if the memory cannot be allocated.</param>
extern HRESULT NF_Runtime_ISR_AllocateMemory(
    InterpreterMemoryType memoryType,
    NF_Runtime_ISR_MemoryOffsetType size,
    bool isISRMemorySpecial,
    void **memory);

/// <summary>
/// Release a block of memory
/// </summary>
/// <param name="memoryType">Type of memory to allocate.</param>
/// <param name="memory">The allocated memory.</param>
/// <returns>The pointer to the memory, or the null pointer if the memory cannot be allocated.</returns>
extern HRESULT NF_Runtime_ISR_ReleaseMemory(
    CONFIGURATION_ARGUMENT_COMMA InterpreterMemoryType memoryType,
    void *memory);

/// <summary>
/// Allocate the memory required to run an RTOS task that runs the service routine
/// that is started from managed code. This method may also be called if the memory already
//  has been allocated for the RTOS task.
/// </summary>
extern HRESULT NF_Runtime_ISR_AllocateManagedActivationTaskData(CONFIGURATION_ARGUMENT);

/// <summary>
/// Release the memory required to run an RTOS task that runs the service routine
/// that is started from managed code. This method may also be called if no memory has
/// been allocated for the RTOS task.
/// </summary>
extern HRESULT NF_Runtime_ISR_ReleaseManagedActivationTaskData(CONFIGURATION_ARGUMENT);

/// <summary>
/// Create a RTOS task to run the service routine.
/// Call the <see cref="NF_Runtime_ISR_RunServiceRoutine"/> method from the task.
/// </summary>
/// <param name="serviceRoutine">Description of the service routine to call.</param>
extern HRESULT NF_Runtime_ISR_StartManagedActivationTask(
    CONFIGURATION_ARGUMENT_COMMA NF_Runtime_ISR_ManagedActivation *serviceRoutine);

/// <summary>
/// Stop the RTOS task started by <see cref="NF_Runtime_ISR_StartManagedActivationTask"/>.
/// </summary>
extern HRESULT NF_Runtime_ISR_StopManagedActivationTask(CONFIGURATION_ARGUMENT);

/// <summary>
/// Allocate the memory required to run an RTOS task that calls service routines after
/// an interrupt has been handled. This method may also be called if the memory already
//  has been allocated for the RTOS task.
/// </summary>
extern HRESULT NF_Runtime_ISR_AllocateRTOSTaskData(CONFIGURATION_ARGUMENT);

/// <summary>
/// Release the memory required to run an RTOS task that calls service routines after
/// an interrupt has been handled. This method may also be called if no memory has
/// been allocated for the RTOS task.
/// </summary>
extern HRESULT NF_Runtime_ISR_ReleaseRTOSTaskData(CONFIGURATION_ARGUMENT);

/// <summary>
/// Enable/start the RTOS task that calls service routines after an interrupt has been handled.
/// </summary>
extern HRESULT NF_Runtime_ISR_EnableRTOSTask(CONFIGURATION_ARGUMENT);
#endif

/// <summary>
/// Put the service routine(s) in a queue to be run in a RTOS task.
/// Call the <see cref="NF_Runtime_ISR_RunFromRTOSTask"/> method from the task.
/// </summary>
/// <param name="interruptHandler">Native data required by the native interpreter to run the service routines.</param>
extern void NF_Runtime_ISR_QueueRTOSTask(NF_Runtime_ISR_InterruptHandler *interruptHandler);

#ifndef NF_RUNTIME_ISR_UNITTESTS
/// <summary>
/// Enable/start the RTOS task that calls service routines after an interrupt has been handled.
/// </summary>
extern HRESULT NF_Runtime_ISR_DisableRTOSTask(CONFIGURATION_ARGUMENT);
#endif

#ifndef NF_RUNTIME_ISR_UNITTESTS
/// <summary>
/// Get the amount of memory required for the mutex (or other semaphore) that provides exclusive access.
/// </summary>
/// <param name="configuration">Device/platform configuration and (native) state for the service routine
/// manager.</param>
extern NF_Runtime_ISR_MemoryOffsetType NF_Runtime_ISR_LockGetMemorySize();

/// <summary>
/// Initialise the mutex (or other semaphore) that provides exclusive access.
/// </summary>
/// <param name="configuration">Device/platform configuration and (native) state for the service routine
/// manager.</param>
/// <param name="mutex">Native data for the mutex (or other semaphore) to use to get exclusive access.</param>
extern HRESULT NF_Runtime_ISR_LockInitialise(void *mutex);

/// <summary>
/// Dispose of the mutex (or other semaphore) that provides exclusive access.
/// </summary>
/// <param name="configuration">Device/platform configuration and (native) state for the service routine
/// manager.</param>
/// <param name="mutex">Native data for the mutex (or other semaphore) used to get exclusive access.</param>
extern HRESULT NF_Runtime_ISR_LockRelease(void *mutex);
#endif

/// <summary>
/// Method to enter the scope of a lock. The method should return only if exclusive access
/// has been obtained.
/// </summary>
/// <param name="mutex">Native data for the mutex (or other semaphore) to use to get exclusive access.
/// It is safe to pass a null pointer; the method then returns immediately.</param>
extern void NF_Runtime_ISR_LockEnterScope(void *mutex);

/// <summary>
/// Method to exit the scope of a lock and release the exclusive access.
/// </summary>
/// <param name="mutex">Native data for the mutex (or other semaphore) used to get exclusive access./// It is safe to
/// pass a null pointer</param>
extern void NF_Runtime_ISR_LockExitScope(void *mutex);

/// <summary>
/// Pause the execution of the service routine.
/// </summary>
/// <param name="time_us">The amount of microseconds to pause.</param>
extern void NF_Runtime_ISR_Sleep(CLR_UINT32 time_us);

#endif // NF_RUNTIME_ISR_PAL_H
