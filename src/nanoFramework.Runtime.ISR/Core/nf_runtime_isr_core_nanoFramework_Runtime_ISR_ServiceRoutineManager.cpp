//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"

//----------------------------------------------------------------------
//
// Service routine manager functionality
//
//----------------------------------------------------------------------

static void *GetMemoryPointer(CLR_RT_StackFrame &stack)
{
    InterpreterMemoryType memoryType = (InterpreterMemoryType)stack.Arg1().NumericByRef().s4;
    int field;
    switch (memoryType)
    {
        case InterpreterMemoryType::InterpreterMemoryType_ISR:
            field = Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryISR;
            break;
        case InterpreterMemoryType::InterpreterMemoryType_Task:
            field = Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryTask;
            break;
        case InterpreterMemoryType::InterpreterMemoryType_ManagedActivation:
            field = Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                FIELD___memoryManagedActivation;
            break;
        default:
            return nullptr;
    }

    CLR_RT_HeapBlock *pThis = stack.This();
    return ARG_AS_INTPTR(pThis[field]);
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DoAllocateMemory___BOOLEAN__nanoFrameworkRuntimeISRInterpreterMemoryType__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        InterpreterMemoryType memoryType = (InterpreterMemoryType)stack.Arg1().NumericByRef().s4;
        NF_Runtime_ISR_MemoryOffsetType size = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
        void *memory = NF_RunTime_ISR_AllocateMemory(memoryType, size);

        bool result = false;
        if (memory != nullptr)
        {
            memset(memory, 0, size);

            int field;
            switch (memoryType)
            {
                case InterpreterMemoryType::InterpreterMemoryType_ISR:
                    field =
                        Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryISR;
                    break;
                case InterpreterMemoryType::InterpreterMemoryType_Task:
                    field =
                        Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryTask;
                    break;
                case InterpreterMemoryType::InterpreterMemoryType_ManagedActivation:
                    field = Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                        FIELD___memoryManagedActivation;
                    break;
                default:
                    NANOCLR_SET_AND_LEAVE(CLR_E_NOTIMPL);
            }

            CLR_RT_HeapBlock *pThis = stack.This();
            FAULT_ON_NULL(pThis);
            SET_AS_INTPTR(pThis[field], memory);

            result = true;
        }

        stack.SetResult_Boolean(result);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    AssignMemory___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    memory += offset;

    CLR_RT_HeapBlock_Array *array = stack.Arg3().DereferenceArray();
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        memcpy(memory, data, array->m_numOfElements);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    AssignOffset___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offsetOffset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType offset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg3());
    *(NF_Runtime_ISR_MemoryOffsetType *)(memory + offsetOffset) = offset;

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    IsOffsetAssigned___BOOLEAN__nanoFrameworkRuntimeISRInterpreterMemoryType__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offsetOffset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    bool isAssigned = 0 != *(NF_Runtime_ISR_MemoryOffsetType *)(memory + offsetOffset);
    stack.SetResult_Boolean(isAssigned);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    GetMemoryPointer___I4__nanoFrameworkRuntimeISRInterpreterMemoryType__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offsetOffset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType offset = *(NF_Runtime_ISR_MemoryOffsetType *)(memory + offsetOffset);
    memory += offset;
    SET_RESULT_AS_INTPTR(memory);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::ReleaseMemory___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        void *memory = ARG_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryISR]);
        if (memory != nullptr)
        {
            NF_RunTime_ISR_ReleaseMemory(InterpreterMemoryType::InterpreterMemoryType_ISR, memory);
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryISR],
                0);
        }

        memory = ARG_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryTask]);
        if (memory != nullptr)
        {
            NF_RunTime_ISR_ReleaseMemory(InterpreterMemoryType::InterpreterMemoryType_Task, memory);
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryTask],
                0);
        }

        memory = ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                         FIELD___memoryManagedActivation]);
        if (memory != nullptr)
        {
            NF_RunTime_ISR_ReleaseMemory(InterpreterMemoryType::InterpreterMemoryType_ManagedActivation, memory);
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___memoryManagedActivation],
                0);
        }

        memory = ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                         FIELD___memoryAfterInterruptTask]);
        if (memory != nullptr)
        {
            NF_RunTime_ISR_ReleaseRTOSTaskData(memory);
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___memoryAfterInterruptTask],
                0);
        }

        memory = ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                         FIELD___memoryManagedActivationTask]);
        if (memory != nullptr)
        {
            NF_RunTime_ISR_ReleaseManagedActivationTaskData(memory);
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___memoryManagedActivationTask],
                0);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoEnableRTOSTask___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        void *taskData =
            ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                    FIELD___memoryAfterInterruptTask]);
        if (taskData == nullptr)
        {
            taskData = NF_RunTime_ISR_AllocateRTOSTaskData();
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___memoryAfterInterruptTask],
                taskData);
        }

        NF_Runtime_ISR_MemoryOffsetType queueSize =
            ARG_AS_MEMORYOFFSETTYPE(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                              FIELD___interruptQueueSize]);

        NF_RunTime_ISR_EnableRTOSTask(taskData, queueSize);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DisableRTOSTask___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        void *taskData =
            ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                    FIELD___memoryAfterInterruptTask]);
        NF_RunTime_ISR_DisableRTOSTask(taskData);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DoStartServiceRoutine___VOID__nanoFrameworkRuntimeISROnManagedActivation__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    CLR_RT_HeapBlock *managedActivationArgument = stack.Arg1().Dereference();

    NF_Runtime_ISR_ManagedActivation managedActivation;
    NF_RunTime_ISR_InitialiseManagedActivation(managedActivation, managedActivationArgument);

    bool inTask = (bool)stack.Arg2().NumericByRef().u1;
    if (inTask)
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        void *taskData =
            ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                    FIELD___memoryManagedActivationTask]);
        if (taskData == nullptr)
        {
            taskData = NF_RunTime_ISR_AllocateManagedActivationTaskData();
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___memoryManagedActivationTask],
                taskData);
        }

        NF_RunTime_ISR_StartManagedActivationTask(taskData, &managedActivation);
    }
    else
    {
        NF_RunTime_ISR_RunServiceRoutine(&managedActivation);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    StopManagedActivatedServiceRoutine___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        void *taskData =
            ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                    FIELD___memoryManagedActivationTask]);
        if (taskData != nullptr)
        {
            NF_RunTime_ISR_StopManagedActivationTask(taskData);
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_GetMemorySize___U4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType result = NF_RunTime_ISR_DataBuffer_GetMemorySize(capacity, elementSize);
    SET_RESULT_AS_MEMORYOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

//----------------------------------------------------------------------
//
// Read/write managed data from/to memory
//
//----------------------------------------------------------------------

static HRESULT ReadManagedData(
    CLR_RT_HeapBlock *instanceArg,
    CLR_UINT8 **dataType,
    bool asArrayElement,
    CLR_UINT8 **data)
{
    NANOCLR_HEADER();

    if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) != 0 && !asArrayElement)
    {
        CLR_UINT8 elementType = ~NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY & **dataType;
        CLR_RT_HeapBlock_Array *array = instanceArg->DereferenceArray();
        if (array == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
        }

        if ((elementType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
        {
            CLR_UINT8 *elementPtr = array->GetFirstElement();
            CLR_UINT8 *elementDataType = *dataType;
            for (CLR_UINT32 iElement = 0; iElement < array->m_numOfElements;
                 iElement++, elementPtr += array->m_sizeOfElement)
            {
                *dataType = elementDataType;
                NANOCLR_CHECK_HRESULT(ReadManagedData((CLR_RT_HeapBlock *)elementPtr, dataType, true, data));
            }
        }
        else
        {
            if (array->m_typeOfElement != elementType)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            CLR_UINT32 numBytes = array->m_numOfElements * array->m_sizeOfElement;
            memcpy(array->GetFirstElement(), *data, numBytes);
            *data += numBytes;
            (*dataType)++;
        }
    }
    else if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
    {
        // instanceArg is an object reference, the referenced heap block (instanceWithFields)
        // is the struct or class instance it references (and that has the data).
        CLR_RT_HeapBlock *instanceWithFields = instanceArg;
        if (instanceArg->DataType() == CLR_DataType::DATATYPE_OBJECT)
        {
            instanceWithFields = instanceArg->Dereference();
        }

        if (instanceWithFields->DataType() != CLR_DataType::DATATYPE_VALUETYPE &&
            instanceWithFields->DataType() != CLR_DataType::DATATYPE_CLASS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        CLR_UINT8 numFields =
            ~(NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT | NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) & *(*dataType)++;
        for (CLR_UINT8 iField = 0; iField < numFields; iField++)
        {
            CLR_UINT8 fieldIndex = *(*dataType)++;
            NANOCLR_CHECK_HRESULT(ReadManagedData(&instanceWithFields[fieldIndex], dataType, false, data));
        }
    }
    else
    {
        switch (*(NF_Runtime_ISR_CompiledDataType *)(*dataType)++)
        {
            case NF_Runtime_ISR_CompiledDataType::Boolean:
                instanceArg->SetBoolean(*(*data)++ != NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_FALSE);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt8:
                instanceArg->SetInteger(*(*data)++);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int8:
                instanceArg->SetInteger(*(CLR_INT8 *)(*data)++);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt16:
                instanceArg->SetInteger(*(CLR_UINT16 *)*data);
                *data += sizeof(CLR_UINT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int16:
                instanceArg->SetInteger(*(CLR_INT16 *)*data);
                *data += sizeof(CLR_INT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt32:
                instanceArg->SetInteger(*(CLR_UINT32 *)*data);
                *data += sizeof(CLR_UINT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int32:
                instanceArg->SetInteger(*(CLR_INT32 *)*data);
                *data += sizeof(CLR_INT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt64:
                instanceArg->SetInteger(*(CLR_UINT64 *)*data);
                *data += sizeof(CLR_UINT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int64:
                instanceArg->SetInteger(*(CLR_INT64 *)*data);
                *data += sizeof(CLR_INT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Single:
                instanceArg->SetFloat(*(float *)*data);
                *data += sizeof(float);
                break;

            case NF_Runtime_ISR_CompiledDataType::Double:
                instanceArg->SetDouble(*(double *)*data);
                *data += sizeof(double);
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_UNSUPPORTED_INSTRUCTION);
        }
    }

    NANOCLR_NOCLEANUP();
}

static HRESULT WriteManagedData(
    CLR_RT_HeapBlock *instanceArg,
    CLR_UINT8 **dataType,
    bool asArrayElement,
    CLR_UINT8 **data)
{
    NANOCLR_HEADER();

    if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) != 0 && !asArrayElement)
    {
        CLR_UINT8 elementType = ~NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY & **dataType;
        CLR_RT_HeapBlock_Array *array = instanceArg->DereferenceArray();
        if (array == NULL)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_NULL_REFERENCE);
        }

        if ((elementType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
        {
            CLR_UINT8 *elementPtr = array->GetFirstElement();
            CLR_UINT8 *elementDataType = *dataType;
            for (CLR_UINT32 iElement = 0; iElement < array->m_numOfElements;
                 iElement++, elementPtr += array->m_sizeOfElement)
            {
                *dataType = elementDataType;
                NANOCLR_CHECK_HRESULT(WriteManagedData((CLR_RT_HeapBlock *)elementPtr, dataType, true, data));
            }
        }
        else
        {
            if (array->m_typeOfElement != elementType)
            {
                NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
            }

            CLR_UINT32 numBytes = array->m_numOfElements * array->m_sizeOfElement;
            memcpy(*data, array->GetFirstElement(), numBytes);
            *data += numBytes;
            (*dataType)++;
        }
    }
    else if ((**dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
    {
        // instanceArg is an object reference, the referenced heap block (instanceWithFields)
        // is the struct or class instance it references (and that has the data).
        CLR_RT_HeapBlock *instanceWithFields = instanceArg;
        if (instanceArg->DataType() == CLR_DataType::DATATYPE_OBJECT)
        {
            instanceWithFields = instanceArg->Dereference();
        }

        if (instanceWithFields->DataType() != CLR_DataType::DATATYPE_VALUETYPE &&
            instanceWithFields->DataType() != CLR_DataType::DATATYPE_CLASS)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_WRONG_TYPE);
        }

        CLR_UINT8 numFields =
            ~(NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT | NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) & *(*dataType)++;
        for (CLR_UINT8 iField = 0; iField < numFields; iField++)
        {
            CLR_UINT8 fieldIndex = *(*dataType)++;
            NANOCLR_CHECK_HRESULT(WriteManagedData(&instanceWithFields[fieldIndex], dataType, false, data));
        }
    }
    else
    {
        switch (*(NF_Runtime_ISR_CompiledDataType *)(*dataType)++)
        {
            case NF_Runtime_ISR_CompiledDataType::Boolean:
                *(*data)++ = instanceArg->NumericByRef().u1 != 0 ? NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_TRUE
                                                                 : NF_RUNTIME_ISR_COMPILEDDATATYPE_BOOLEAN_FALSE;
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt8:
                *(*data)++ = instanceArg->NumericByRef().u1;
                break;

            case NF_Runtime_ISR_CompiledDataType::Int8:
                *(CLR_UINT8 *)(*data)++ = instanceArg->NumericByRef().s1;
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt16:
                *(CLR_UINT16 *)*data = instanceArg->NumericByRef().u2;
                *data += sizeof(CLR_UINT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int16:
                *(CLR_INT16 *)*data = instanceArg->NumericByRef().s2;
                *data += sizeof(CLR_INT16);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt32:
                *(CLR_UINT32 *)*data = instanceArg->NumericByRef().u4;
                *data += sizeof(CLR_UINT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int32:
                *(CLR_INT32 *)*data = instanceArg->NumericByRef().s4;
                *data += sizeof(CLR_INT32);
                break;

            case NF_Runtime_ISR_CompiledDataType::UInt64:
                *(CLR_UINT64 *)*data = (CLR_UINT64)instanceArg->NumericByRef().u8;
                *data += sizeof(CLR_UINT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Int64:
                *(CLR_INT64 *)*data = (CLR_INT64)instanceArg->NumericByRef().s8;
                *data += sizeof(CLR_INT64);
                break;

            case NF_Runtime_ISR_CompiledDataType::Single:
                *(float *)*data = instanceArg->NumericByRef().r4;
                *data += sizeof(float);
                break;

            case NF_Runtime_ISR_CompiledDataType::Double:
                *(double *)*data = (double)instanceArg->NumericByRef().r8;
                *data += sizeof(double);
                break;

            default:
                NANOCLR_SET_AND_LEAVE(CLR_E_UNSUPPORTED_INSTRUCTION);
        }
    }

    NANOCLR_NOCLEANUP();
}

static HRESULT ReadWriteManagedData(
    HRESULT (*readWrite)(CLR_RT_HeapBlock *instanceArg, CLR_UINT8 **dataType, bool asArrayElement, CLR_UINT8 **data),
    CLR_RT_HeapBlock &instanceArg,
    CLR_RT_HeapBlock &dataTypeArg,
    CLR_UINT8 *data)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataType = (CLR_UINT8 *)dataTypeArg.DereferenceArray()->GetFirstElement();
    if ((*dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISARRAY) != 0)
    {
        // instanceArg is an CLR_DataType::DATATYPE_SZARRAY representing the array.
        NANOCLR_CHECK_HRESULT(readWrite(&instanceArg, &dataType, false, &data));
    }
    else if ((*dataType & NF_RUNTIME_ISR_COMPILEDDATATYPE_ISDATASTRUCT) != 0)
    {
        // instanceArg is an CLR_DataType::DATATYPE_OBJECT referencing a CLR_DataType::DATATYPE_VALUETYPE
        // for structs and a CLR_DataType::DATATYPE_CLASS for a class instance. As fields that point to
        // a struct/class instance also are of type CLR_DataType::DATATYPE_OBJECT, the CLR_DataType::DATATYPE_OBJECT
        // instance should be passed as argument.
        NANOCLR_CHECK_HRESULT(readWrite(&instanceArg, &dataType, false, &data));
    }
    else
    {
        // instanceArg is an CLR_DataType::DATATYPE_OBJECT that references a CLR_DataType::DATATYPE_VALUETYPE
        // The value is the first field of the latter.
        NANOCLR_CHECK_HRESULT(readWrite(&instanceArg.Dereference()[1], &dataType, false, &data));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    ReadManagedData___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__OBJECT__SZARRAY_U1(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    memory += offset;

    NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(ReadManagedData, stack.Arg3(), stack.Arg4(), memory));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    WriteManagedData___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__OBJECT__SZARRAY_U1(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    memory += offset;

    NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(WriteManagedData, stack.Arg3(), stack.Arg4(), memory));

    NANOCLR_NOCLEANUP();
}

//----------------------------------------------------------------------
//
// Data buffers
//
//----------------------------------------------------------------------

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_Initialize___VOID__I4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg3());

    NF_RunTime_ISR_DataBuffer_Initialize(dataBuffer, capacity, elementSize);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Capacity___U4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType capacity = NF_RunTime_ISR_DataBuffer_Capacity(dataBuffer);
    SET_RESULT_AS_MEMORYOFFSETTYPE(capacity);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Count___U4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType count = NF_RunTime_ISR_DataBuffer_Count(dataBuffer);
    SET_RESULT_AS_MEMORYOFFSETTYPE(count);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_Add___VOID__I4__OBJECT__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    CLR_UINT8 *memory = NF_RunTime_ISR_DataBuffer_Add(dataBuffer, nullptr);
    if (memory != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(WriteManagedData, stack.Arg2(), stack.Arg3(), memory));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_Insert___VOID__I4__U4__OBJECT__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType index = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    CLR_UINT8 *memory = NF_RunTime_ISR_DataBuffer_Insert(dataBuffer, index, nullptr);
    if (memory != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(WriteManagedData, stack.Arg3(), stack.Arg4(), memory));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_Get___VOID__I4__U4__OBJECT__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType index = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    CLR_UINT8 *memory = NF_RunTime_ISR_DataBuffer_Get(dataBuffer, index, nullptr);
    if (memory != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(ReadManagedData, stack.Arg3(), stack.Arg4(), memory));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Clear___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_RunTime_ISR_DataBuffer_Clear(dataBuffer);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_GetMemorySize___U4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType result = NF_RunTime_ISR_DataRingBuffer_GetMemorySize(capacity, elementSize);
    SET_RESULT_AS_MEMORYOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_Initialize___VOID__I4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg3());

    NF_RunTime_ISR_DataRingBuffer_Initialize(dataBuffer, capacity, elementSize);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Capacity___U4__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType capacity = NF_RunTime_ISR_DataRingBuffer_Capacity(dataBuffer);
    SET_RESULT_AS_MEMORYOFFSETTYPE(capacity);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_IsEmpty___BOOLEAN__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    bool isEmpty = NF_RunTime_ISR_DataRingBuffer_IsEmpty(dataBuffer);
    stack.SetResult_Boolean(isEmpty);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_Push___BOOLEAN__I4__OBJECT__SZARRAY_U1__BOOLEAN(CLR_RT_StackFrame &stack)
{
    // TODO
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    bool allowOverwrite = stack.Arg4().NumericByRef().s4 != 0;
    bool success;
    CLR_UINT8 *memory = NF_RunTime_ISR_DataRingBuffer_Push(dataBuffer, nullptr, allowOverwrite, success);
    stack.SetResult_Boolean(success);
    if (memory != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(WriteManagedData, stack.Arg2(), stack.Arg3(), memory));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_Peek___BOOLEAN__I4__OBJECT__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    bool isAvailable;
    CLR_UINT8 *memory = NF_RunTime_ISR_DataRingBuffer_PeekPop(dataBuffer, nullptr, false, isAvailable);
    stack.SetResult_Boolean(isAvailable);
    if (memory != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(ReadManagedData, stack.Arg2(), stack.Arg3(), memory));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_Pop___BOOLEAN__I4__OBJECT__SZARRAY_U1(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    bool isAvailable;
    CLR_UINT8 *memory = NF_RunTime_ISR_DataRingBuffer_PeekPop(dataBuffer, nullptr, true, isAvailable);
    stack.SetResult_Boolean(isAvailable);
    if (memory != nullptr)
    {
        NANOCLR_SET_AND_LEAVE(ReadWriteManagedData(ReadManagedData, stack.Arg2(), stack.Arg3(), memory));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Clear___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_RunTime_ISR_DataRingBuffer_Clear(dataBuffer);

    NANOCLR_NOCLEANUP_NOLABEL();
}
