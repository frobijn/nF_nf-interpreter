//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

//======================================================================
//
// This code file for the ISR interpreter is shared by the nf-interpreter
// and the nanoFramework.Runtime.ISR.Interpreter project. If this file
// is modified in one git repository, make sure to copy it to to the
// other repository as well.
// 
//======================================================================

#include "NF_Runtime_ISR.h"
#include "string.h"

//----------------------------------------------------------------------
//
// Access to data buffers
// 
//----------------------------------------------------------------------
#pragma region Data buffer
CLR_UINT8 *NF_RunTime_ISR_DataBuffer_GetMemory (CLR_UINT8 *memory, NF_Runtime_ISR_SharedDataOffsetType offsetOffset)
{
    return memory + *(NF_Runtime_ISR_SharedDataOffsetType *)(memory + offsetOffset);
}

NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_DataBuffer_GetMemorySize (NF_Runtime_ISR_SharedDataOffsetType capacity, NF_Runtime_ISR_HeapOffsetType elementSize)
{
    // Extra memory is to store capacity, element size, and element count.
    return 2 * (NF_Runtime_ISR_SharedDataOffsetType)sizeof (NF_Runtime_ISR_SharedDataOffsetType) + (NF_Runtime_ISR_SharedDataOffsetType)sizeof (NF_Runtime_ISR_HeapOffsetType) + capacity * elementSize;
}

void NF_RunTime_ISR_DataBuffer_Initialize (CLR_UINT8 *dataBuffer, NF_Runtime_ISR_SharedDataOffsetType capacity, NF_Runtime_ISR_HeapOffsetType elementSize)
{
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = capacity;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer = elementSize;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = 0;
}

NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_DataBuffer_Capacity (CLR_UINT8 *dataBuffer)
{
    return *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
}

NF_Runtime_ISR_HeapOffsetType NF_RunTime_ISR_DataBuffer_ElementSize (CLR_UINT8 *dataBuffer)
{
    return *(NF_Runtime_ISR_HeapOffsetType *)(dataBuffer + sizeof (NF_Runtime_ISR_SharedDataOffsetType));
}

NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_DataBuffer_Count (CLR_UINT8 *dataBuffer)
{
    return *(NF_Runtime_ISR_SharedDataOffsetType *)(dataBuffer + sizeof (NF_Runtime_ISR_SharedDataOffsetType) + sizeof (NF_Runtime_ISR_HeapOffsetType));
}

void NF_RunTime_ISR_DataBuffer_Add (CLR_UINT8 *dataBuffer, CLR_UINT8 *data)
{
    NF_Runtime_ISR_SharedDataOffsetType capacity = *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_HeapOffsetType elementSize = *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType *count = (NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);

    if (*count < capacity)
    {
        dataBuffer += elementSize * (*count)++;
        memcpy (dataBuffer, data, elementSize);
    }
}

void NF_RunTime_ISR_DataBuffer_Insert (CLR_UINT8 *dataBuffer, NF_Runtime_ISR_SharedDataOffsetType index, CLR_UINT8 *data)
{
    NF_Runtime_ISR_SharedDataOffsetType capacity = *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_HeapOffsetType elementSize = *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType *count = (NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);

    if (index < capacity)
    {
        if (index == *count)
        {
            (*count)++;
        }

        if (index < *count)
        {
            dataBuffer += elementSize * index;
            memcpy (dataBuffer, data, elementSize);
        }
    }
}

void NF_RunTime_ISR_DataBuffer_Get (CLR_UINT8 *dataBuffer, NF_Runtime_ISR_SharedDataOffsetType index, CLR_UINT8 *data)
{
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_HeapOffsetType elementSize = *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType count = *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);

    if (index < count)
    {
        dataBuffer += elementSize * index;
        memcpy (data, dataBuffer, elementSize);
    }
}

void NF_RunTime_ISR_DataBuffer_Clear (CLR_UINT8 *dataBuffer)
{
    *(NF_Runtime_ISR_SharedDataOffsetType *)(dataBuffer + sizeof (NF_Runtime_ISR_SharedDataOffsetType) + sizeof (NF_Runtime_ISR_HeapOffsetType)) = 0;
}
#pragma endregion

#pragma region Ring buffer
NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_DataRingBuffer_GetMemorySize (NF_Runtime_ISR_SharedDataOffsetType capacity, NF_Runtime_ISR_HeapOffsetType elementSize)
{
    // Extra memory is to store capacity, element size, index to write, index to read, and the empty flag.
    return 3 * (NF_Runtime_ISR_SharedDataOffsetType)sizeof (NF_Runtime_ISR_SharedDataOffsetType) + (NF_Runtime_ISR_SharedDataOffsetType)sizeof (NF_Runtime_ISR_HeapOffsetType) + 1 + capacity * elementSize;
}

NF_Runtime_ISR_HeapOffsetType NF_RunTime_ISR_DataRingBuffer_ElementSize (CLR_UINT8 *dataBuffer)
{
    return *(NF_Runtime_ISR_HeapOffsetType *)(dataBuffer + sizeof (NF_Runtime_ISR_SharedDataOffsetType) + 1);
}

void NF_RunTime_ISR_DataRingBuffer_Initialize (CLR_UINT8 *dataBuffer, NF_Runtime_ISR_SharedDataOffsetType capacity, NF_Runtime_ISR_HeapOffsetType elementSize)
{
    *dataBuffer++ = 0xFF;
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = capacity;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer = elementSize;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = 0;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = 0;
}

NF_Runtime_ISR_SharedDataOffsetType NF_RunTime_ISR_DataRingBuffer_Capacity (CLR_UINT8 *dataBuffer)
{
    return *(NF_Runtime_ISR_SharedDataOffsetType *)++dataBuffer;
}

bool NF_RunTime_ISR_DataRingBuffer_IsEmpty (CLR_UINT8 *dataBuffer)
{
    return *dataBuffer != 0;
}

bool NF_RunTime_ISR_DataRingBuffer_Push (CLR_UINT8 *dataBuffer, CLR_UINT8 *data, bool allowOverwrite)
{
    CLR_UINT8 *isEmpty = dataBuffer++;
    NF_Runtime_ISR_SharedDataOffsetType capacity = *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_HeapOffsetType elementSize = *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType *indexToWriteData = (NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType *indexToReadData = (NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);

    NF_Runtime_ISR_SharedDataOffsetType nextIndexToWrite = (1 + *indexToWriteData) % capacity;

    if (*isEmpty != 0)
    {
        *isEmpty = 0;
    }
    else if (*indexToWriteData == *indexToReadData)
    {
        if (!allowOverwrite)
        {
            return false;
        }

        *indexToReadData = nextIndexToWrite;
    }

    dataBuffer += elementSize * *indexToWriteData;
    *indexToWriteData = nextIndexToWrite;
    memcpy (dataBuffer, data, elementSize);

    return true;
}



bool NF_RunTime_ISR_DataRingBuffer_PeekPop (CLR_UINT8 *dataBuffer, CLR_UINT8 *data, bool removeFromBuffer)
{
    CLR_UINT8 *isEmpty = dataBuffer++;
    if (*isEmpty != 0)
    {
        return false;
    }

    NF_Runtime_ISR_SharedDataOffsetType capacity = *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_HeapOffsetType elementSize = *(NF_Runtime_ISR_HeapOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_HeapOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType indexToWriteData = *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    NF_Runtime_ISR_SharedDataOffsetType *indexToReadData = (NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);

    dataBuffer += elementSize * *indexToReadData;
    memcpy (data, dataBuffer, elementSize);

    if (removeFromBuffer)
    {
        *indexToReadData = (1 + *indexToReadData) % capacity;
        if (indexToWriteData == *indexToReadData)
        {
            *isEmpty = 0xFF;
        }
    }

    return true;
}

void NF_RunTime_ISR_DataRingBuffer_Clear (CLR_UINT8 *dataBuffer)
{
    *dataBuffer++ = 0xFF;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType) + sizeof (NF_Runtime_ISR_HeapOffsetType);
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = 0;
    dataBuffer += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
    *(NF_Runtime_ISR_SharedDataOffsetType *)dataBuffer = 0;
}
#pragma endregion

//----------------------------------------------------------------------
//
// Running service routines and access to memory
// 
//----------------------------------------------------------------------

#pragma region Memory access
typedef struct __nfpack ServiceRoutineMemory
{
    CLR_UINT8 *OnInterruptMemory;
    CLR_UINT8 *AfterInterruptMemory;
    CLR_UINT8 *ManagedActivationMemory;
    CLR_UINT8 *Heap;
} ServiceRoutineMemory;

static CLR_UINT8 *ReadUnifiedMemoryPointer (CLR_UINT8 **bytecodePtr, ServiceRoutineMemory &memory)
{
    CLR_UINT8 *result;
    // Unified memory addresses/offsets are stored as big endian
    if ((**bytecodePtr & NF_RUNTIME_ISR_MEMORY_HEAP_FLAG) != 0)
    {
        NF_Runtime_ISR_HeapOffsetType offset = (*(CLR_UINT8 *)(*bytecodePtr)++) & NF_RUNTIME_ISR_MEMORY_HEAP_MASK;
        offset <<= 8;
        offset |= *(*bytecodePtr)++;

        if ((offset & NF_RUNTIME_ISR_MEMORY_HEAP_INDIRECT_REFERENCE_FLAG) != 0)
        {
            offset &= NF_RUNTIME_ISR_MEMORY_HEAP_INDIRECT_REFERENCE_MASK;
            result = memory.Heap + offset;
            return ReadUnifiedMemoryPointer (&result, memory);
        }
        else
        {
            result = memory.Heap + offset;
        }
    }
    else
    {
        NF_Runtime_ISR_SharedDataOffsetType offset = *(CLR_UINT8 *)(*bytecodePtr)++;
        offset <<= 8;
        offset |= *(*bytecodePtr)++;
        offset <<= 8;
        offset |= *(*bytecodePtr)++;
        offset <<= 8;
        offset |= *(*bytecodePtr)++;

        switch (offset & NF_RUNTIME_ISR_MEMORY_OFFSET_BITS)
        {
        case NF_RUNTIME_ISR_MEMORY_OFFSET_ISR_FLAG:
            result = memory.OnInterruptMemory;
            break;
        case NF_RUNTIME_ISR_MEMORY_OFFSET_TASK_FLAG:
            result = memory.AfterInterruptMemory;
            break;
        default:
            result = memory.ManagedActivationMemory;
            break;
        }

        offset &= NF_RUNTIME_ISR_MEMORY_OFFSET_MASK;
        if ((offset & NF_RUNTIME_ISR_MEMORY_SHAREDDATA_INDIRECT_REFERENCE_FLAG) != 0)
        {
            offset &= NF_RUNTIME_ISR_MEMORY_SHAREDDATA_INDIRECT_REFERENCE_MASK;
            offset = *(NF_Runtime_ISR_SharedDataOffsetType *)(result + offset);
        }

        result += offset;
    }

    return result;
}
#pragma endregion

#pragma region Service routines and tasks
void NF_RunTime_ISR_HandleInterrupt (NF_Runtime_ISR_InterruptHandler *interruptHandler, NF_Runtime_ISR_ServiceParameterType eventArg)
{
    if (interruptHandler->OnInterruptSetArgumentsOffset != 0 || interruptHandler->OnInterruptOffset != 0)
    {
        NF_Runtime_ISR_ServiceRoutine serviceRoutine;
        serviceRoutine.CalledFromTask = false;
        serviceRoutine.EventArg = eventArg;
        serviceRoutine.InterruptHandler = interruptHandler;

        if (interruptHandler->OnInterruptSetArgumentsOffset != 0)
        {
            serviceRoutine.ServiceRoutineOffset = interruptHandler->OnInterruptSetArgumentsOffset;
            if (!NF_RunTime_ISR_RunServiceRoutine (&serviceRoutine))
            {
                return;
            }
            serviceRoutine.EventArg = 0;
        }

        if (interruptHandler->OnInterruptOffset != 0)
        {
            serviceRoutine.ServiceRoutineOffset = interruptHandler->OnInterruptOffset;
            if (!NF_RunTime_ISR_RunServiceRoutine (&serviceRoutine)) {
                return;
            }
        }
    }

    if (interruptHandler->AfterInterruptSetArgumentsOffset != 0 || interruptHandler->AfterInterruptOffset != 0)
    {
        NF_Runtime_ISR_ServiceParameterType taskDataOffset = *(NF_Runtime_ISR_ServiceParameterType *)(interruptHandler->AfterInterruptMemory + NF_Runtime_ISR_ServiceParameterTypeSize);
        NF_RunTime_ISR_QueueRTOSTask (interruptHandler, interruptHandler->AfterInterruptMemory + taskDataOffset);
    }
}

void NF_RunTime_ISR_RunFromRTOSTask (NF_Runtime_ISR_InterruptHandler *interruptHandler)
{
    if (interruptHandler->AfterInterruptSetArgumentsOffset != 0 || interruptHandler->AfterInterruptOffset != 0)
    {
        NF_Runtime_ISR_ServiceRoutine serviceRoutine{};
        serviceRoutine.CalledFromTask = true;
        serviceRoutine.EventArg = 0;
        serviceRoutine.InterruptHandler = interruptHandler;

        if (interruptHandler->AfterInterruptSetArgumentsOffset != 0)
        {
            serviceRoutine.ServiceRoutineOffset = interruptHandler->AfterInterruptSetArgumentsOffset;
            if (!NF_RunTime_ISR_RunServiceRoutine (&serviceRoutine))
            {
                return;
            }
        }

        if (interruptHandler->AfterInterruptOffset != 0)
        {
            serviceRoutine.ServiceRoutineOffset = interruptHandler->AfterInterruptOffset;
            NF_RunTime_ISR_RunServiceRoutine (&serviceRoutine);
        }
    }
}

#ifdef NF_RUNTIME_ISR_FOR_NANOCLR
static bool RunServiceRoutine (ServiceRoutineMemory &memory, CLR_UINT8 *bytecodePtr, NF_Runtime_ISR_ServiceParameterType eventArg, NF_RunTime_ISR_RaiseManagedEvent raiseEvent, CLR_INT32 serviceManagerId);
#else
static bool RunServiceRoutine (ServiceRoutineMemory &memory, CLR_UINT8 *bytecodePtr, NF_Runtime_ISR_ServiceParameterType eventArg, NF_RunTime_ISR_RaiseManagedEvent raiseEvent);
#endif

void NF_RunTime_ISR_RunServiceRoutine (NF_Runtime_ISR_ManagedActivation *serviceRoutine)
{
    if (serviceRoutine == nullptr || serviceRoutine->ServiceRoutineOffset == 0)
    {
        return;
    }

    ServiceRoutineMemory memory{};
    memory.OnInterruptMemory = serviceRoutine->OnInterruptMemory;
    memory.AfterInterruptMemory = serviceRoutine->AfterInterruptMemory;
    memory.ManagedActivationMemory = serviceRoutine->ServiceRoutineMemory;
    memory.Heap = memory.ManagedActivationMemory + *(NF_Runtime_ISR_SharedDataOffsetType *)memory.ManagedActivationMemory;

    CLR_UINT8 *bytecodePtr;
    switch (serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_BITS)
    {
    case NF_RUNTIME_ISR_MEMORY_OFFSET_ISR_FLAG:
        bytecodePtr = memory.OnInterruptMemory + (serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_MASK);
        break;
    case NF_RUNTIME_ISR_MEMORY_OFFSET_TASK_FLAG:
        bytecodePtr = memory.AfterInterruptMemory + (serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_MASK);
        break;
    default:
        bytecodePtr = memory.ManagedActivationMemory + (serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_MASK);
        break;
    }

#ifdef NF_RUNTIME_ISR_FOR_NANOCLR
    RunServiceRoutine (memory, bytecodePtr, 0, serviceRoutine->RaiseManagedEvent, serviceRoutine->ServiceManagerId);
#else
    RunServiceRoutine (memory, bytecodePtr, 0, serviceRoutine->RaiseManagedEvent);
#endif
}

bool NF_RunTime_ISR_RunServiceRoutine (NF_Runtime_ISR_ServiceRoutine *serviceRoutine)
{
    if (serviceRoutine == nullptr || serviceRoutine->ServiceRoutineOffset == 0)
    {
        return true;
    }

    ServiceRoutineMemory memory{};
    memory.OnInterruptMemory = serviceRoutine->InterruptHandler->OnInterruptMemory;
    memory.AfterInterruptMemory = serviceRoutine->CalledFromTask ? serviceRoutine->InterruptHandler->AfterInterruptMemory : nullptr;
    memory.ManagedActivationMemory = nullptr;

    if (serviceRoutine->CalledFromTask)
    {
        memory.Heap = memory.AfterInterruptMemory + *(NF_Runtime_ISR_SharedDataOffsetType *)memory.AfterInterruptMemory;
    }
    else
    {
        memory.Heap = memory.OnInterruptMemory + *(NF_Runtime_ISR_SharedDataOffsetType *)memory.OnInterruptMemory;
    }

    CLR_UINT8 *bytecodePtr;
    if ((serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_BITS) == NF_RUNTIME_ISR_MEMORY_OFFSET_TASK_FLAG)
    {
        bytecodePtr = memory.AfterInterruptMemory + (serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_MASK);
    }
    else
    {
        bytecodePtr = memory.OnInterruptMemory + (serviceRoutine->ServiceRoutineOffset & NF_RUNTIME_ISR_MEMORY_OFFSET_MASK);
    }

#ifdef NF_RUNTIME_ISR_FOR_NANOCLR
    return RunServiceRoutine (memory, bytecodePtr, serviceRoutine->EventArg, serviceRoutine->InterruptHandler->RaiseManagedEvent, serviceRoutine->InterruptHandler->ServiceManagerId);
#else
    return RunServiceRoutine (memory, bytecodePtr, serviceRoutine->EventArg, serviceRoutine->InterruptHandler->RaiseManagedEvent);
#endif
}
#pragma endregion

//----------------------------------------------------------------------
//
// Interpreter of the byte code of the service routines
// 
//----------------------------------------------------------------------

#ifdef NF_RUNTIME_ISR_FOR_NANOCLR
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

#ifdef NF_RUNTIME_ISR_FOR_NANOCLR
static bool RunServiceRoutine (ServiceRoutineMemory &memory, CLR_UINT8 *bytecodePtr, NF_Runtime_ISR_ServiceParameterType eventArg, NF_RunTime_ISR_RaiseManagedEvent raiseEvent, CLR_INT32 serviceManagerId)
#else
static bool RunServiceRoutine (ServiceRoutineMemory &memory, CLR_UINT8 *bytecodePtr, NF_Runtime_ISR_ServiceParameterType eventArg, NF_RunTime_ISR_RaiseManagedEvent raiseEvent)
#endif
{
#pragma region Interpretation of byte code
    while (true)
    {
        NF_Runtime_ISR_CompiledOpCode opCode = *(NF_Runtime_ISR_CompiledOpCode *)bytecodePtr++;

        switch (opCode)
        {
#pragma region Program flow
        case NF_Runtime_ISR_CompiledOpCode::Return:
            return true;

        case NF_Runtime_ISR_CompiledOpCode::Abort:
            return false;

        case NF_Runtime_ISR_CompiledOpCode::ReturnOrAbort:
            return 0 != *ReadUnifiedMemoryPointer (&bytecodePtr, memory);

        case NF_Runtime_ISR_CompiledOpCode::GoTo:
            bytecodePtr += *(NF_Runtime_ISR_ByteCodeOffsetType *)bytecodePtr - 1;
            break;

        case NF_Runtime_ISR_CompiledOpCode::OnConditionGoTo:
        {
            CLR_UINT8 *goTo = bytecodePtr + *(NF_Runtime_ISR_ByteCodeOffsetType *)bytecodePtr - 1;
            bytecodePtr += sizeof (NF_Runtime_ISR_ByteCodeOffsetType);
            if (0 != *ReadUnifiedMemoryPointer (&bytecodePtr, memory))
            {
                bytecodePtr = goTo;
            }
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::OnNotConditionGoTo:
        {
            CLR_UINT8 *goTo = bytecodePtr + *(NF_Runtime_ISR_ByteCodeOffsetType *)bytecodePtr - 1;
            bytecodePtr += sizeof (NF_Runtime_ISR_ByteCodeOffsetType);
            if (0 == *ReadUnifiedMemoryPointer (&bytecodePtr, memory))
            {
                bytecodePtr = goTo;
            }
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::ManagedEventRaise:
#ifdef NF_RUNTIME_ISR_FOR_NANOCLR
            raiseEvent (serviceManagerId, *(CLR_UINT32 *)ReadUnifiedMemoryPointer (&bytecodePtr, memory));
#else
            raiseEvent (*(CLR_UINT32 *)ReadUnifiedMemoryPointer (&bytecodePtr, memory));
#endif
            break;
#pragma endregion

#pragma region Memory operations
        case NF_Runtime_ISR_CompiledOpCode::StoreLiteral:
        {
            CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 numBytes = *bytecodePtr++;
            memcpy (toLocation, bytecodePtr, numBytes);
            bytecodePtr += numBytes;
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::StoreZero:
        {
            CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_Runtime_ISR_HeapOffsetType numBytes = *(NF_Runtime_ISR_HeapOffsetType *)bytecodePtr;
            bytecodePtr += sizeof (NF_Runtime_ISR_HeapOffsetType);
            memset (toLocation, 0, numBytes);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::CopyData:
        {
            CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *fromLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_Runtime_ISR_HeapOffsetType numBytes = *(NF_Runtime_ISR_HeapOffsetType *)bytecodePtr;
            bytecodePtr += sizeof (NF_Runtime_ISR_HeapOffsetType);
            memcpy (toLocation, fromLocation, numBytes);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::CopyTypecastData:
        {
            // Only selected typecasts are implemented in SourceCodeCompiler, based on the little endianness
            // and the fact that the bit representation does not change in a typecast signed <=> unsigned.
            // Overflows are ignored.
            // We only need to have small signed => large signed and small unsigned => large unsigned conversions.
            // For same sized integers, the SourceCodeCompiler uses CopyData.
            // For large integer => small integer, the SourceCodeCompiler uses CopyData.
            // For small signed => large  integer, the SourceCodeCompiler uses signed => signed.
            // For small unsigned => large signed, the SourceCodeCompiler uses unsigned => unsigned.

            CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *fromLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            switch (*bytecodePtr++)
            {
            case ((int)NF_Runtime_ISR_CompiledDataType::UInt16 + (((int)NF_Runtime_ISR_CompiledDataType::UInt8) << 4)):
                *(CLR_UINT16 *)toLocation = (CLR_UINT16)(*(CLR_UINT8 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::UInt32 + (((int)NF_Runtime_ISR_CompiledDataType::UInt8) << 4)):
                *(CLR_UINT32 *)toLocation = (CLR_UINT32)(*(CLR_UINT8 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::UInt32 + (((int)NF_Runtime_ISR_CompiledDataType::UInt16) << 4)):
                *(CLR_UINT32 *)toLocation = (CLR_UINT32)(*(CLR_UINT16 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::UInt64 + (((int)NF_Runtime_ISR_CompiledDataType::UInt8) << 4)):
                *(CLR_UINT64 *)toLocation = (CLR_UINT64)(*(CLR_UINT8 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::UInt64 + (((int)NF_Runtime_ISR_CompiledDataType::UInt16) << 4)):
                *(CLR_UINT64 *)toLocation = (CLR_UINT64)(*(CLR_UINT16 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::UInt64 + (((int)NF_Runtime_ISR_CompiledDataType::UInt32) << 4)):
                *(CLR_UINT64 *)toLocation = (CLR_UINT64)(*(CLR_UINT32 *)fromLocation);
                break;

            case ((int)NF_Runtime_ISR_CompiledDataType::Int16 + (((int)NF_Runtime_ISR_CompiledDataType::Int8) << 4)):
                *(CLR_INT16 *)toLocation = (CLR_INT16)(*(CLR_INT8 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::Int32 + (((int)NF_Runtime_ISR_CompiledDataType::Int8) << 4)):
                *(CLR_INT32 *)toLocation = (CLR_INT32)(*(CLR_INT8 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::Int32 + (((int)NF_Runtime_ISR_CompiledDataType::Int16) << 4)):
                *(CLR_INT32 *)toLocation = (CLR_INT32)(*(CLR_INT16 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::Int64 + (((int)NF_Runtime_ISR_CompiledDataType::Int8) << 4)):
                *(CLR_INT64 *)toLocation = (CLR_INT64)(*(CLR_INT8 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::Int64 + (((int)NF_Runtime_ISR_CompiledDataType::Int16) << 4)):
                *(CLR_INT64 *)toLocation = (CLR_INT64)(*(CLR_INT16 *)fromLocation);
                break;
            case ((int)NF_Runtime_ISR_CompiledDataType::Int64 + (((int)NF_Runtime_ISR_CompiledDataType::Int32) << 4)):
                *(CLR_INT64 *)toLocation = (CLR_INT64)(*(CLR_INT32 *)fromLocation);
                break;
            }
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::CopyParameter:
        {
            CLR_UINT8 numBytes = *bytecodePtr++;
            memcpy (memory.Heap, &eventArg, numBytes);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::UpdateHeapOffset:
        {
            CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *fromLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_Runtime_ISR_HeapOffsetType elementSize = *(NF_Runtime_ISR_HeapOffsetType *)bytecodePtr;
            bytecodePtr += sizeof (NF_Runtime_ISR_HeapOffsetType);
            // Offset is stored in big endian
            NF_Runtime_ISR_HeapOffsetType offset = 0;
            for (int i = 0; i < sizeof (NF_Runtime_ISR_HeapOffsetType); i++)
            {
                ((CLR_UINT8 *)&offset)[sizeof (NF_Runtime_ISR_HeapOffsetType) - i - 1] = toLocation[i];
            }
            offset += elementSize * *(NF_Runtime_ISR_HeapOffsetType *)fromLocation;
            for (int i = 0; i < sizeof (NF_Runtime_ISR_HeapOffsetType); i++)
            {
                toLocation[i] = ((CLR_UINT8 *)&offset)[sizeof (NF_Runtime_ISR_HeapOffsetType) - i - 1];
            }
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::UpdateSharedDataOffset:
        {
            CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *fromLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_Runtime_ISR_SharedDataOffsetType elementSize = *(NF_Runtime_ISR_SharedDataOffsetType *)bytecodePtr;
            bytecodePtr += sizeof (NF_Runtime_ISR_SharedDataOffsetType);
            // Offset is stored in big endian
            NF_Runtime_ISR_SharedDataOffsetType offset = 0;
            for (int i = 0; i < sizeof (NF_Runtime_ISR_SharedDataOffsetType); i++)
            {
                ((CLR_UINT8 *)&offset)[sizeof (NF_Runtime_ISR_SharedDataOffsetType) - i - 1] = toLocation[i];
            }
            offset += elementSize * *(NF_Runtime_ISR_SharedDataOffsetType *)fromLocation;
            for (int i = 0; i < sizeof (NF_Runtime_ISR_SharedDataOffsetType); i++)
            {
                toLocation[i] = ((CLR_UINT8 *)&offset)[sizeof (NF_Runtime_ISR_SharedDataOffsetType) - i - 1];
            }
            break;
        }
#pragma endregion

#pragma region Data buffers
        case NF_Runtime_ISR_CompiledOpCode::DataBufferGetCapacity:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *returnValue = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            *(NF_Runtime_ISR_SharedDataOffsetType *)returnValue = NF_RunTime_ISR_DataBuffer_Capacity (dataBuffer);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataBufferGetCount:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *returnValue = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            *(NF_Runtime_ISR_SharedDataOffsetType *)returnValue = NF_RunTime_ISR_DataBuffer_Count (dataBuffer);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataBufferAdd:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *data = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_RunTime_ISR_DataBuffer_Add (dataBuffer, data);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataBufferInsert:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_Runtime_ISR_SharedDataOffsetType index = *(NF_Runtime_ISR_SharedDataOffsetType *)ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *data = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_RunTime_ISR_DataBuffer_Insert (dataBuffer, index, data);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataBufferGet:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_Runtime_ISR_SharedDataOffsetType index = *(NF_Runtime_ISR_SharedDataOffsetType *)ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *data = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_RunTime_ISR_DataBuffer_Get (dataBuffer, index, data);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataBufferClear:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_RunTime_ISR_DataBuffer_Clear (dataBuffer);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferGetCapacity:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *returnValue = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            *(NF_Runtime_ISR_SharedDataOffsetType *)returnValue = NF_RunTime_ISR_DataRingBuffer_Capacity (dataBuffer);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferIsEmpty:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *returnValue = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            *(NF_Runtime_ISR_SharedDataOffsetType *)returnValue = NF_RunTime_ISR_DataRingBuffer_IsEmpty (dataBuffer);
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPush:
        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPushNoReturn:
        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPushOverwrite:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *data = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            bool overwrite = opCode == NF_Runtime_ISR_CompiledOpCode::DataRingBufferPushOverwrite
                || 0 != *ReadUnifiedMemoryPointer (&bytecodePtr, memory);

            bool success = NF_RunTime_ISR_DataRingBuffer_Push (dataBuffer, data, overwrite);

            if (opCode == NF_Runtime_ISR_CompiledOpCode::DataRingBufferPush)
            {
                *ReadUnifiedMemoryPointer (&bytecodePtr, memory) = success ? 0xFF : 0;
            }
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPeek:
        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPop:
        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPeekNoReturn:
        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferPopNoReturn:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            CLR_UINT8 *data = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            bool remove = opCode == NF_Runtime_ISR_CompiledOpCode::DataRingBufferPop;

            bool success = NF_RunTime_ISR_DataRingBuffer_PeekPop (dataBuffer, data, remove);

            if (opCode == NF_Runtime_ISR_CompiledOpCode::DataRingBufferPeek || opCode == NF_Runtime_ISR_CompiledOpCode::DataRingBufferPop)
            {
                *ReadUnifiedMemoryPointer (&bytecodePtr, memory) = success ? 0xFF : 0;
            }
            break;
        }

        case NF_Runtime_ISR_CompiledOpCode::DataRingBufferClear:
        {
            CLR_UINT8 *dataBuffer = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
            NF_RunTime_ISR_DataRingBuffer_Clear (dataBuffer);
            break;
        }
#pragma endregion

        default:
        {
            if (opCode >= NF_Runtime_ISR_CompiledOpCode::DataBusRead)
            {
#pragma region Data bus
                NF_Runtime_ISR_DataBus *dataBus = (NF_Runtime_ISR_DataBus *)ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                CLR_UINT8 *buffer1Location = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                NF_Runtime_ISR_HeapOffsetType buffer1Size = *(NF_Runtime_ISR_HeapOffsetType *)bytecodePtr;
                bytecodePtr += sizeof (NF_Runtime_ISR_HeapOffsetType);
                CLR_UINT8 *resultLocation = nullptr;

                switch (opCode)
                {
                case NF_Runtime_ISR_CompiledOpCode::DataBusReadWithResult:
                    resultLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                case NF_Runtime_ISR_CompiledOpCode::DataBusRead:
                    dataBus->Read (dataBus, buffer1Location, buffer1Size, resultLocation);
                    break;

                case NF_Runtime_ISR_CompiledOpCode::DataBusWriteWithResult:
                    resultLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                case NF_Runtime_ISR_CompiledOpCode::DataBusWrite:
                    dataBus->Write (dataBus, buffer1Location, buffer1Size, resultLocation);
                    break;

                case NF_Runtime_ISR_CompiledOpCode::DataBusWriteRead:
                case NF_Runtime_ISR_CompiledOpCode::DataBusWriteReadWithResult:
                    CLR_UINT8 *buffer2Location = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                    NF_Runtime_ISR_HeapOffsetType buffer2Size = *(NF_Runtime_ISR_HeapOffsetType *)bytecodePtr;
                    bytecodePtr += sizeof (NF_Runtime_ISR_HeapOffsetType);
                    if (opCode == NF_Runtime_ISR_CompiledOpCode::DataBusWriteReadWithResult)
                    {
                        resultLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                    }
                    dataBus->WriteRead (dataBus, buffer1Location, buffer1Size, buffer2Location, buffer2Size, resultLocation);
                    break;
                }
#pragma endregion
            }
            else if (opCode >= NF_Runtime_ISR_CompiledOpCode::AddInteger8)
            {
#pragma region Integer and boolean operations
                if (opCode <= NF_Runtime_ISR_CompiledOpCode::XorInteger64)
                {
                    CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                    CLR_UINT8 *fromLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);

#define INTEGER_MATH_BITWISE_OPERATOR(op,type) *(type *)toLocation op *(type *)fromLocation

                    switch (opCode)
                    {
                    case NF_Runtime_ISR_CompiledOpCode::AddInteger8: INTEGER_MATH_BITWISE_OPERATOR (+=, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::AddInteger16: INTEGER_MATH_BITWISE_OPERATOR (+=, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::AddInteger32: INTEGER_MATH_BITWISE_OPERATOR (+=, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::AddInteger64: INTEGER_MATH_BITWISE_OPERATOR (+=, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::SubtractInteger8: INTEGER_MATH_BITWISE_OPERATOR (-=, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::SubtractInteger16: INTEGER_MATH_BITWISE_OPERATOR (-=, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::SubtractInteger32: INTEGER_MATH_BITWISE_OPERATOR (-=, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::SubtractInteger64: INTEGER_MATH_BITWISE_OPERATOR (-=, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::MultiplyInteger8: INTEGER_MATH_BITWISE_OPERATOR (*=, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::MultiplyInteger16: INTEGER_MATH_BITWISE_OPERATOR (*=, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::MultiplyInteger32: INTEGER_MATH_BITWISE_OPERATOR (*=, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::MultiplyInteger64: INTEGER_MATH_BITWISE_OPERATOR (*=, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::DivideInteger8: INTEGER_MATH_BITWISE_OPERATOR (/=, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::DivideInteger16: INTEGER_MATH_BITWISE_OPERATOR (/=, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::DivideInteger32: INTEGER_MATH_BITWISE_OPERATOR (/=, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::DivideInteger64: INTEGER_MATH_BITWISE_OPERATOR (/=, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::RemainderInteger8: INTEGER_MATH_BITWISE_OPERATOR (%=, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::RemainderInteger16: INTEGER_MATH_BITWISE_OPERATOR (%=, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::RemainderInteger32: INTEGER_MATH_BITWISE_OPERATOR (%=, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::RemainderInteger64: INTEGER_MATH_BITWISE_OPERATOR (%=, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::AndInteger8: INTEGER_MATH_BITWISE_OPERATOR (&=, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::AndInteger16: INTEGER_MATH_BITWISE_OPERATOR (&=, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::AndInteger32: INTEGER_MATH_BITWISE_OPERATOR (&=, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::AndInteger64: INTEGER_MATH_BITWISE_OPERATOR (&=, CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::OrInteger8: INTEGER_MATH_BITWISE_OPERATOR (|=, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::OrInteger16: INTEGER_MATH_BITWISE_OPERATOR (|=, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::OrInteger32: INTEGER_MATH_BITWISE_OPERATOR (|=, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::OrInteger64: INTEGER_MATH_BITWISE_OPERATOR (|=, CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::XorInteger8: INTEGER_MATH_BITWISE_OPERATOR (^=, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::XorInteger16: INTEGER_MATH_BITWISE_OPERATOR (^=, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::XorInteger32: INTEGER_MATH_BITWISE_OPERATOR (^=, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::XorInteger64: INTEGER_MATH_BITWISE_OPERATOR (^=, CLR_UINT64); break;
                    }
                }
                else if (opCode <= NF_Runtime_ISR_CompiledOpCode::ArithmeticShiftRightInteger64)
                {
                    CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                    CLR_UINT8 *fromLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);

#define INTEGER_SHIFT_OPERATOR(op,type) *(type *)toLocation op *fromLocation

                    switch (opCode)
                    {
                    case NF_Runtime_ISR_CompiledOpCode::ShiftLeftInteger8: INTEGER_SHIFT_OPERATOR (<<=, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::ShiftLeftInteger16: INTEGER_SHIFT_OPERATOR (<<=, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::ShiftLeftInteger32: INTEGER_SHIFT_OPERATOR (<<=, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::ShiftLeftInteger64: INTEGER_SHIFT_OPERATOR (<<=, CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::LogicalShiftRightInteger8: INTEGER_SHIFT_OPERATOR (>>=, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::LogicalShiftRightInteger16: INTEGER_SHIFT_OPERATOR (>>=, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::LogicalShiftRightInteger32: INTEGER_SHIFT_OPERATOR (>>=, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::LogicalShiftRightInteger64: INTEGER_SHIFT_OPERATOR (>>=, CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::ArithmeticShiftRightInteger8: INTEGER_SHIFT_OPERATOR (>>=, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::ArithmeticShiftRightInteger16: INTEGER_SHIFT_OPERATOR (>>=, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::ArithmeticShiftRightInteger32: INTEGER_SHIFT_OPERATOR (>>=, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::ArithmeticShiftRightInteger64: INTEGER_SHIFT_OPERATOR (>>=, CLR_INT64); break;
                    }
                }
                else if (opCode <= NF_Runtime_ISR_CompiledOpCode::UnaryPlusPlusInteger64)
                {
                    CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);

#define INTEGER_UNARY_OPERATOR(op,type) *(type *)toLocation = op (*(type *)toLocation)
#define INTEGER_UNARY_OPERATOR2(op,type) { type x = *(type *)toLocation; op x; *(type *)toLocation = x; }

                    switch (opCode)
                    {
                    case NF_Runtime_ISR_CompiledOpCode::ComplementInteger8: INTEGER_UNARY_OPERATOR (~, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::ComplementInteger16: INTEGER_UNARY_OPERATOR (~, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::ComplementInteger32: INTEGER_UNARY_OPERATOR (~, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::ComplementInteger64: INTEGER_UNARY_OPERATOR (~, CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusInteger8: INTEGER_UNARY_OPERATOR (-, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusInteger16: INTEGER_UNARY_OPERATOR (-, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusInteger32: INTEGER_UNARY_OPERATOR (-, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusInteger64: INTEGER_UNARY_OPERATOR (-, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusMinusInteger8: INTEGER_UNARY_OPERATOR2 (--, CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusMinusInteger16: INTEGER_UNARY_OPERATOR2 (--, CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusMinusInteger32: INTEGER_UNARY_OPERATOR2 (--, CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryMinusMinusInteger64: INTEGER_UNARY_OPERATOR2 (--, CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryPlusPlusInteger8: INTEGER_UNARY_OPERATOR2 (++, CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryPlusPlusInteger16: INTEGER_UNARY_OPERATOR2 (++, CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryPlusPlusInteger32: INTEGER_UNARY_OPERATOR2 (++, CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::UnaryPlusPlusInteger64: INTEGER_UNARY_OPERATOR2 (++, CLR_UINT64); break;
                    }
                }
                else if (opCode <= NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualSignedInteger64)
                {
                    CLR_UINT8 *toLocation = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                    CLR_UINT8 *value1Location = ReadUnifiedMemoryPointer (&bytecodePtr, memory);
                    CLR_UINT8 *value2Location = ReadUnifiedMemoryPointer (&bytecodePtr, memory);

#define COMPARISON_OPERATOR(op,type) *(CLR_UINT8 *)toLocation = (*(type *)value1Location op *(type *)value2Location) ? 0xFF : 0

                    switch (opCode)
                    {
                    case NF_Runtime_ISR_CompiledOpCode::AreEqualInteger8: COMPARISON_OPERATOR (== , CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreEqualInteger16: COMPARISON_OPERATOR (== , CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreEqualInteger32: COMPARISON_OPERATOR (== , CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreEqualInteger64: COMPARISON_OPERATOR (== , CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreNotEqualInteger8: COMPARISON_OPERATOR (!= , CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreNotEqualInteger16: COMPARISON_OPERATOR (!= , CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreNotEqualInteger32: COMPARISON_OPERATOR (!= , CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::AreNotEqualInteger64: COMPARISON_OPERATOR (!= , CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualSignedInteger8: COMPARISON_OPERATOR (<= , CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualSignedInteger16: COMPARISON_OPERATOR (<= , CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualSignedInteger32: COMPARISON_OPERATOR (<= , CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualSignedInteger64: COMPARISON_OPERATOR (<= , CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualUnsignedInteger8: COMPARISON_OPERATOR (<= , CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualUnsignedInteger16: COMPARISON_OPERATOR (<= , CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualUnsignedInteger32: COMPARISON_OPERATOR (<= , CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessOrEqualUnsignedInteger64: COMPARISON_OPERATOR (<= , CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanSignedInteger8: COMPARISON_OPERATOR (< , CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanSignedInteger16: COMPARISON_OPERATOR (< , CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanSignedInteger32: COMPARISON_OPERATOR (< , CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanSignedInteger64: COMPARISON_OPERATOR (< , CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanUnsignedInteger8: COMPARISON_OPERATOR (< , CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanUnsignedInteger16: COMPARISON_OPERATOR (< , CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanUnsignedInteger32: COMPARISON_OPERATOR (< , CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsLessThanUnsignedInteger64: COMPARISON_OPERATOR (< , CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualSignedInteger8: COMPARISON_OPERATOR (>= , CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualSignedInteger16: COMPARISON_OPERATOR (>= , CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualSignedInteger32: COMPARISON_OPERATOR (>= , CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualSignedInteger64: COMPARISON_OPERATOR (>= , CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualUnsignedInteger8: COMPARISON_OPERATOR (>= , CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualUnsignedInteger16: COMPARISON_OPERATOR (>= , CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualUnsignedInteger32: COMPARISON_OPERATOR (>= , CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterOrEqualUnsignedInteger64: COMPARISON_OPERATOR (>= , CLR_UINT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanSignedInteger8: COMPARISON_OPERATOR (> , CLR_INT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanSignedInteger16: COMPARISON_OPERATOR (> , CLR_INT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanSignedInteger32: COMPARISON_OPERATOR (> , CLR_INT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanSignedInteger64: COMPARISON_OPERATOR (> , CLR_INT64); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanUnsignedInteger8: COMPARISON_OPERATOR (> , CLR_UINT8); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanUnsignedInteger16: COMPARISON_OPERATOR (> , CLR_UINT16); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanUnsignedInteger32: COMPARISON_OPERATOR (> , CLR_UINT32); break;
                    case NF_Runtime_ISR_CompiledOpCode::IsGreaterThanUnsignedInteger64: COMPARISON_OPERATOR (> , CLR_UINT64); break;
                    }
                }
#pragma endregion
            }
        }

        }
    }
#pragma endregion
}
