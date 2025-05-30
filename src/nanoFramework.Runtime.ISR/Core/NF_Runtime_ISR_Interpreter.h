//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//======================================================================
//
// This header for the ISR interpreter is shared by the nf-interpreter
// and the nanoFramework.Runtime.ISR.Interpreter project. If this file
// is modified in one git repository, make sure to copy it to to the
// other repository as well.
//
//======================================================================

#ifndef NF_RUNTIME_ISR_INTERPRETER_H
#define NF_RUNTIME_ISR_INTERPRETER_H

#include "NF_Runtime_ISR.h"

//----------------------------------------------------------------------
//
// Interrupt handlers and service routines
//
//----------------------------------------------------------------------

/// <summary>
/// Pointers to the methods of a data bus to be used by the interpreter
/// to read/write data to the data bus. The struct (or an extended version of it)
/// should be initialised by a data bus implementation. The data bus implementation
/// should not allocate additional data. If additional data is required, it should
/// declare a struct that has a <c>NF_Runtime_ISR_DataBus</c> as first field,
/// and return the size of that struct in the <c>IDataBus.GetNativeMemorySize</c> implementation.
/// The struct should be initialised in <c>IDataBus.InitialiseNativeDataBus</c>.
/// </summary>
typedef struct __nfpack NF_Runtime_ISR_DataBus
{
    /// <summary>
    /// Method to read a number of bytes from the data bus.
    /// </summary>
    /// <param name="dataBus">A pointer to this structure.</param>
    /// <param name="dataPtr">A pointer to the memory to write the data to.</param>
    /// <param name="dataSize">The number of bytes to read.</param>
    /// <param name="result">If the <c>Read*</c> method of the data bus specification (in the
    /// <c>nanoFramework.Runtime.ISR.Specification</c> namespace) has declared a return type, and that return type is
    /// used in the code of the service routine: a pointer to the (uninitialized) memory to write the result to.
    /// Otherwise <see langword="nullptr" /> is passed.</param>
    void (*Read)(
        struct NF_Runtime_ISR_DataBus *dataBus,
        CLR_UINT8 *dataPtr,
        NF_Runtime_ISR_MemoryOffsetType dataSize,
        void *result);

    /// <summary>
    /// Method to write a number of bytes to the data bus.
    /// </summary>
    /// <param name="dataBus">A pointer to this structure.</param>
    /// <param name="dataPtr">A pointer to the memory to read the data from.</param>
    /// <param name="dataSize">The number of bytes to write.</param>
    /// <param name="result">If the <c>Write*</c> method of the data bus specification (in the
    /// <c>nanoFramework.Runtime.ISR.Specification</c> namespace) has declared a return type, and that return type is
    /// used in the code of the service routine: a pointer to the (uninitialized) memory to write the result to.
    /// Otherwise <see langword="nullptr" /> is passed.</param>
    void (*Write)(
        struct NF_Runtime_ISR_DataBus *dataBus,
        CLR_UINT8 *dataPtr,
        NF_Runtime_ISR_MemoryOffsetType dataSize,
        void *result);

    /// <summary>
    /// Method to write and read a number of bytes to the data bus in one operation.
    /// </summary>
    /// <param name="dataBus">A pointer to this structure.</param>
    /// <param name="writeDataPtr">A pointer to the memory to read the data from.</param>
    /// <param name="writeDataSize">The number of bytes to write.</param>
    /// <param name="readDataPtr">A pointer to the memory to write the data to.</param>
    /// <param name="readDataSize">The number of bytes to read.</param>
    /// <param name="result">If the <c>Write*Read*</c> method of the data bus specification (in the
    /// <c>nanoFramework.Runtime.ISR.Specification</c> namespace) has declared a return type, and that return type is
    /// used in the code of the service routine: a pointer to the (uninitialized) memory to write the result to.
    /// Otherwise <see langword="nullptr" /> is passed.</param>
    void (*WriteRead)(
        struct NF_Runtime_ISR_DataBus *dataBus,
        CLR_UINT8 *writeDataPtr,
        NF_Runtime_ISR_MemoryOffsetType writeDataSize,
        CLR_UINT8 *readDataPtr,
        NF_Runtime_ISR_MemoryOffsetType readDataSize,
        void *result);

} NF_Runtime_ISR_DataBus;

/// <summary>
/// Method (implemented by the native <c>ServiceRoutineManager.InterpreterBase</c>) that should be called
/// to raise an event that is handled in managed code.
/// </summary>
#ifdef NF_RUNTIME_ISR_UNITTESTS
typedef void (*NF_RunTime_ISR_RaiseManagedEvent)(CLR_UINT32 eventArg);
#else
typedef void (*NF_RunTime_ISR_RaiseManagedEvent)(CLR_INT32 managerId, CLR_UINT32 eventArg);
#endif

/// <summary>
/// Specification of the interrupt handlers. If an interrupt handler requires additional data,
/// declare a struct that has a <c>NF_Runtime_ISR_InterruptHandler</c> as first field,
/// and return the size of that struct in the <c>IInterruptGenerator.GetNativeMemorySize</c> implementation.
/// The struct should be initialised in <c>IInterruptGenerator.Enable</c>.
/// </summary>
typedef struct __nfpack NF_Runtime_ISR_InterruptHandler
{
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._onInterruptMemory</c> property value.</summary>
    CLR_UINT8 *OnInterruptMemory;
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._onInterruptSetArgumentsOffset</c> property
    /// value.</summary>
    NF_Runtime_ISR_MemoryOffsetType OnInterruptSetArgumentsOffset;
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._onInterruptOffset</c> property value.</summary>
    NF_Runtime_ISR_MemoryOffsetType OnInterruptOffset;
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._afterInterruptMemory</c> property
    /// value.</summary>
    CLR_UINT8 *AfterInterruptMemory;
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._afterInterruptSetArgumentsOffset</c> property
    /// value.</summary>
    NF_Runtime_ISR_MemoryOffsetType AfterInterruptSetArgumentsOffset;
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._afterInterruptOffset</c> property
    /// value.</summary>
    NF_Runtime_ISR_MemoryOffsetType AfterInterruptOffset;
#ifndef NF_RUNTIME_ISR_UNITTESTS
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._taskMemory</c> property value.</summary>
    void *TaskMemory;
    /// <summary>Initialise this value with the <c>OnInterruptHandlers._managerId</c> property value.</summary>
    CLR_UINT32 ServiceManagerId;
#endif
    /// <summary>Method to raise an event that will be handled in managed code.</summary>
    NF_RunTime_ISR_RaiseManagedEvent RaiseManagedEvent;
} NF_Runtime_ISR_InterruptHandler;

#ifndef NF_RUNTIME_ISR_UNITTESTS
/// <summary>
/// Method to initialise the interrupt handler struct from an argument of type
/// <c>nanoFramework.Runtime.ISR.OnInterruptHandlers</c> that is passed by managed code as part of the
/// <c>nanoFramework.Runtime.ISR.IInterruptGenerator.Enable</c> implementation.
/// </summary>
/// <param name="interruptHandler">Native data that the interrupt generator needs to pass
/// to <see cref="NF_RunTime_ISR_HandleInterrupt"/> in the interrupt handler.</param>
/// <param name="onInterruptHandlerArgument">Argument pass by the managed code.</param>
extern void NF_RunTime_ISR_InitialiseInterruptHandler(
    NF_Runtime_ISR_InterruptHandler &interruptHandler,
    CLR_RT_HeapBlock *onInterruptHandlerArgument);
#endif

/// <summary>
/// Use the native interpreter to run the service routine(s) in response to an interrupt.
/// This method should be called directly from an interrupt handler.
/// </summary>
/// <param name="interruptHandler">Specification of the service routine(s) to run.</param>
/// <param name="eventArg">Optional argument to pass to the first service routine to be called.</param>
extern void NF_RunTime_ISR_HandleInterrupt(
    NF_Runtime_ISR_InterruptHandler *interruptHandler,
    NF_Runtime_ISR_ServiceParameterType eventArg);

/// <summary>
/// Run the service routine(s) from a RTOS task.
/// </summary>
/// <param name="interruptHandler">Specification of the service routine(s) to run.</param>
extern void NF_RunTime_ISR_RunFromRTOSTask(NF_Runtime_ISR_InterruptHandler *interruptHandler);

/// <summary>
/// Specification of the parameters required to run a service routine.
/// </summary>
typedef struct __nfpack NF_Runtime_ISR_ServiceRoutine
{
    /// <summary>Initialise this value with the interrupt handlers passed to
    /// <see cref="NF_RunTime_ISR_HandleInterrupt"/>.</summary>
    NF_Runtime_ISR_InterruptHandler *InterruptHandler;
    /// <summary>Indicates whether the service routine is called from an RTOS task instead of directly from the
    /// interrupt handler.</summary>
    bool CalledFromTask;
    /// <summary>Pass the <c>NF_Runtime_ISR_InterruptHandler.*Offset</c> corresponding to the service routine.</summary>
    NF_Runtime_ISR_MemoryOffsetType ServiceRoutineOffset;
    /// <summary>Optional argument to pass to the service routine.</summary>
    NF_Runtime_ISR_ServiceParameterType EventArg;
} NF_Runtime_ISR_ServiceRoutine;

/// <summary>
/// Use the native interpreter to run a single service routine.
/// </summary>
/// <param name="serviceRoutine">Description of the service routine to call.</param>
/// <returns>Indicates whether the execution of service routines should continue (<see langword="true"/>) or not
/// (<see langword="false"/>).</returns>
extern bool NF_RunTime_ISR_RunServiceRoutine(NF_Runtime_ISR_ServiceRoutine *serviceRoutine);

/// <summary>
/// Specification of the interrupt handlers. If an interrupt handler requires additional data,
/// declare a struct that has a <c>NF_Runtime_ISR_InterruptHandler</c> as first field,
/// and return the size of that struct in the <c>IInterruptGenerator.GetNativeMemorySize</c> implementation.
/// The struct should be initialised in <c>IInterruptGenerator.Enable</c>.
/// </summary>
typedef struct __nfpack NF_Runtime_ISR_ManagedActivation
{
    /// <summary>Initialise this value with the <c>OnManagedActivation._onInterruptMemory</c> property value.</summary>
    CLR_UINT8 *OnInterruptMemory;
    /// <summary>Initialise this value with the <c>OnManagedActivation._afterInterruptMemory</c> property
    /// value.</summary>
    CLR_UINT8 *AfterInterruptMemory;
    /// <summary>Initialise this value with the <c>OnManagedActivation._activationMemory</c> property value.</summary>
    CLR_UINT8 *ServiceRoutineMemory;
    /// <summary>Initialise this value with the <c>OnManagedActivation._serviceRoutineOffset</c> property
    /// value.</summary>
    NF_Runtime_ISR_MemoryOffsetType ServiceRoutineOffset;
#ifndef NF_RUNTIME_ISR_UNITTESTS
    /// <summary>Initialise this value with the <c>OnManagedActivation._managerId</c> property value.</summary>
    CLR_UINT32 ServiceManagerId;
#endif
    /// <summary>Method to raise an event that will be handled in managed code.</summary>
    NF_RunTime_ISR_RaiseManagedEvent RaiseManagedEvent;
} NF_Runtime_ISR_ManagedActivation;

#ifndef NF_RUNTIME_ISR_UNITTESTS
/// <summary>
/// Method to initialise the struct from an argument of type <c>nanoFramework.Runtime.ISR.ManagedActivation</c> that is
/// passed by managed code.
/// </summary>
/// <param name="managedActivation">Native data to pass to <see cref="NF_RunTime_ISR_RunServiceRoutine"/>.</param>
/// <param name="Argument">Argument pass by the managed code.</param>
extern void NF_RunTime_ISR_InitialiseManagedActivation(
    NF_Runtime_ISR_ManagedActivation &managedActivation,
    CLR_RT_HeapBlock *managedActivationArgument);
#endif

/// <summary>
/// Use the native interpreter to run a single service routine.
/// </summary>
/// <param name="serviceRoutine">Description of the service routine to call.</param>
extern void NF_RunTime_ISR_RunServiceRoutine(NF_Runtime_ISR_ManagedActivation *serviceRoutine);

//----------------------------------------------------------------------
//
// Access to data buffers from managed code
//
//----------------------------------------------------------------------

/// <summary>
/// Get the memory reserved for any of the data buffers.
/// </summary>
/// <param name="memory">Pointer to the memory that contains the data for the buffer.</param>
/// <param name="offsetOffset">Offset in the memory to the location where memory for the buffer has been
/// reserved.</param>
extern CLR_UINT8 *NF_RunTime_ISR_DataBuffer_GetMemory(
    CLR_UINT8 *memory,
    NF_Runtime_ISR_MemoryOffsetType offsetOffset);

/// <summary>
/// Get the required memory size for the data buffer.
/// </summary>
/// <param name="capacity">The number of elements that can be stored in the buffer.</param>
/// <param name="elementSize">The size in bytes of an element of the buffer.</param>
/// <returns>The total amount of memory required.</returns>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataBuffer_GetMemorySize(
    NF_Runtime_ISR_MemoryOffsetType capacity,
    NF_Runtime_ISR_MemoryOffsetType elementSize);

/// <summary>
/// Initialise the data buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="capacity">The number of elements that can be stored in the buffer.</param>
/// <param name="elementSize">The size in bytes of an element of the buffer.</param>
extern void NF_RunTime_ISR_DataBuffer_Initialize(
    CLR_UINT8 *dataBuffer,
    NF_Runtime_ISR_MemoryOffsetType capacity,
    NF_Runtime_ISR_MemoryOffsetType elementSize);

/// <summary>
/// Get the number of elements that can be stored in the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataBuffer_Capacity(CLR_UINT8 *dataBuffer);

/// <summary>
/// Get the size in bytes of an element of the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataBuffer_ElementSize(CLR_UINT8 *dataBuffer);

/// <summary>
/// Get the number of elements that are present in the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataBuffer_Count(CLR_UINT8 *dataBuffer);

/// <summary>
/// Add data to the buffer. No data is added if the buffer is already at capacity.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="data">Data to add to the buffer. Pass a null pointer if the caller
/// uses the returned pointer to pass the data.</param>
/// <returns>Pointer to the memory where the data has to be copied to.
/// A null pointer is returned if the buffer is at capacity or if <paramref name="data"/>
/// is not a null pointer.</returns>    
extern CLR_UINT8 *NF_RunTime_ISR_DataBuffer_Add(CLR_UINT8 *dataBuffer, CLR_UINT8 *data);

/// <summary>
/// Add data to the buffer. No data is added if the buffer is already at capacity.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="index">Position in the buffer to store the data.</param>
/// <param name="data">Data to add to the buffer. Pass a null pointer if the caller
/// uses the returned pointer to pass the data.</param>
/// <returns>Pointer to the memory where the data has to be copied to.
/// A null pointer is returned if the buffer is at capacity or if <paramref name="data"/>
/// is not a null pointer.</returns>
extern CLR_UINT8 *NF_RunTime_ISR_DataBuffer_Insert(
    CLR_UINT8 *dataBuffer,
    NF_Runtime_ISR_MemoryOffsetType index,
    CLR_UINT8 *data);

/// <summary>
/// Get data from the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="index">Position in the buffer to get the data from.</param>
/// <param name="data">Location to copy the data to. Pass a null pointer if the caller
/// uses the returned pointer to get the data.</param>
/// <returns>Pointer to the memory where the data has to be copied from.
/// A null pointer is returned if <paramref name="data"/> is not a null pointer.</returns>
extern CLR_UINT8 *NF_RunTime_ISR_DataBuffer_Get(
    CLR_UINT8 *dataBuffer,
    NF_Runtime_ISR_MemoryOffsetType index,
    CLR_UINT8 *data);

/// <summary>
/// Clear the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern void NF_RunTime_ISR_DataBuffer_Clear(CLR_UINT8 *dataBuffer);

/// <summary>
/// Get the required memory size for the data ring buffer.
/// </summary>
/// <param name="capacity">The number of elements that can be stored in the buffer.</param>
/// <param name="elementSize">The size in bytes of an element of the buffer.</param>
/// <returns>The total amount of memory required.</returns>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataRingBuffer_GetMemorySize(
    NF_Runtime_ISR_MemoryOffsetType capacity,
    NF_Runtime_ISR_MemoryOffsetType elementSize);

/// <summary>
/// Initialise the data ring buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="capacity">The number of elements that can be stored in the buffer.</param>
/// <param name="elementSize">The size in bytes of an element of the buffer.</param>
extern void NF_RunTime_ISR_DataRingBuffer_Initialize(
    CLR_UINT8 *dataBuffer,
    NF_Runtime_ISR_MemoryOffsetType capacity,
    NF_Runtime_ISR_MemoryOffsetType elementSize);

/// <summary>
/// Get the number of elements that can be stored in the ring buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataRingBuffer_Capacity(CLR_UINT8 *dataBuffer);

/// <summary>
/// Get the size in bytes of an element of the ring buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern NF_Runtime_ISR_MemoryOffsetType NF_RunTime_ISR_DataRingBuffer_ElementSize(CLR_UINT8 *dataBuffer);

/// <summary>
/// Indicates whether there are no more messages available.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern bool NF_RunTime_ISR_DataRingBuffer_IsEmpty(CLR_UINT8 *dataBuffer);

/// <summary>
/// Add data to the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="data">Data to add to the buffer. Pass a null pointer if the caller
/// uses the returned pointer to pass the data.</param>
/// <param name="allowOverwrite">Indicates whether the data is added to the ring buffer
/// even if it overwrites existing data.</param>
/// <param name="pushed">On return: indicates whether the data has been added. Returns <see langword="false"/>
/// if <paramref name="allowOverwrite"/> is <see langword="false"/> and the buffer is filled
/// to capacity.</param>
/// <returns>Pointer to the memory where the data has to be copied to.
/// A null pointer is returned if the buffer is at capacity and <paramref name="allowOverwrite"/> is <see langword="false"/>,
/// or if <paramref name="data"/> is not a null pointer.</returns>
extern CLR_UINT8 *NF_RunTime_ISR_DataRingBuffer_Push(
    CLR_UINT8 *dataBuffer,
    CLR_UINT8 *data,
    bool allowOverwrite,
    bool &pushed);

/// <summary>
/// Get the next available element in the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
/// <param name="data">Location to copy the data to. Pass a null pointer if the caller
/// uses the returned pointer to get the data.</param>
/// <param name="removeFromBuffer">Indicates whether to remove the data (pop) rather than keep it (peek).</param>
/// <param name="isAvailable">On return: indicates whether any data was available. If <see langword="false"/>, the
/// <paramref name="data"/> has an undefined value and does not represent any data read from
/// the buffer.</param>
/// <returns>Pointer to the memory where the data has to be copied from.
/// A null pointer is returned if <paramref name="data"/> is not a null pointer or <paramref name="data"/> is false.</returns>
extern CLR_UINT8 *NF_RunTime_ISR_DataRingBuffer_PeekPop(
    CLR_UINT8 *dataBuffer,
    CLR_UINT8 *data,
    bool removeFromBuffer,
    bool &isAvailable);

/// <summary>
/// Clear the buffer.
/// </summary>
/// <param name="dataBuffer">Pointer to the memory for the buffer as returned by
/// <see cref="NF_RunTime_ISR_DataBuffer_GetMemory"/>.</param>
extern void NF_RunTime_ISR_DataRingBuffer_Clear(CLR_UINT8 *dataBuffer);

#endif // NF_RUNTIME_ISR_INTERPRETER_H
