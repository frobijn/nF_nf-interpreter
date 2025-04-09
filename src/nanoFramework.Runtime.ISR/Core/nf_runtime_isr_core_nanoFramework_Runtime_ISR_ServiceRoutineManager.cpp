//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoAllocateMemory___BOOLEAN__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__BYREF_I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    InterpreterMemoryType memoryType = (InterpreterMemoryType)stack.Arg1().NumericByRef().s4;
    NF_Runtime_ISR_SharedDataOffsetType size = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    void *memory = NF_RunTime_ISR_AllocateMemory(memoryType, size);

    bool result = false;
    if (memory != nullptr)
    {
        CLR_RT_HeapBlock hMemory;
        hMemory.SetInteger((CLR_INT32)memory);
        NANOCLR_CHECK_HRESULT(hMemory.StoreToReference(stack.Arg3(), 0));
        result = true;
    }
    stack.SetResult_Boolean(result);

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoAssignMemory___VOID__I4__U4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType offset = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    memory += offset;

    CLR_RT_HeapBlock_Array *array = stack.Arg3().DereferenceArray();
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        memcpy(memory, data, array->m_numOfElements);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoAssignOffset___VOID__I4__U4__U4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType offsetOffset = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_SharedDataOffsetType offset = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg3());
    *(NF_Runtime_ISR_SharedDataOffsetType *)(memory + offsetOffset) = offset;

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoIsOffsetAssigned___BOOLEAN__I4__U4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType offsetOffset = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    bool isAssigned = 0 != *(NF_Runtime_ISR_SharedDataOffsetType *)(memory + offsetOffset);
    stack.SetResult_Boolean(isAssigned);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoGetMemoryPointer___I4__I4__U4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType offset = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    memory += offset;
    SET_RESULT_AS_INTPTR(memory);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoReadMemory___VOID__I4__U4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType offset = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    memory += offset;
    CLR_RT_HeapBlock_Array *array = stack.Arg3().DereferenceArray();
    if (array != NULL)
    {
        CLR_UINT8 *result = (CLR_UINT8 *)array->GetFirstElement();
        memcpy(result, memory, array->m_numOfElements);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoReleaseMemory___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    InterpreterMemoryType memoryType = (InterpreterMemoryType)stack.Arg1().NumericByRef().s4;
    void *memory = ARG_AS_INTPTR(stack.Arg2());
    NF_RunTime_ISR_ReleaseMemory(memoryType, memory);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DoStartServiceRoutine___VOID__nanoFrameworkRuntimeISROnManagedActivation( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *managedActivationArgument = stack.Arg1().Dereference();
    NF_Runtime_ISR_ManagedActivation managedActivation;
    NF_RunTime_ISR_InitialiseManagedActivation(managedActivation, managedActivationArgument);
    NF_RunTime_ISR_RunServiceRoutineInSeparateRTOSTask (&managedActivation);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_GetMemorySize___U4__U4__U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_SharedDataOffsetType capacity = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg1());
    NF_Runtime_ISR_HeapOffsetType elementSize = ARG_AS_HEAPOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_SharedDataOffsetType result = NF_RunTime_ISR_DataBuffer_GetMemorySize(capacity, elementSize);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Initialize___VOID__I4__U4__U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType capacity = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_HeapOffsetType elementSize = ARG_AS_HEAPOFFSETTYPE(stack.Arg3());

    NF_RunTime_ISR_DataBuffer_Initialize(dataBuffer, capacity, elementSize);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Capacity___U4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType capacity = NF_RunTime_ISR_DataBuffer_Capacity(dataBuffer);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(capacity);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Count___U4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType count = NF_RunTime_ISR_DataBuffer_Count(dataBuffer);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(count);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Add___VOID__I4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    CLR_RT_HeapBlock_Array *array = stack.Arg2().DereferenceArray();
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        NF_RunTime_ISR_DataBuffer_Add(dataBuffer, data);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Insert___VOID__I4__U4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType index = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    CLR_RT_HeapBlock_Array *array = stack.Arg3().DereferenceArray();
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        NF_RunTime_ISR_DataBuffer_Insert(dataBuffer, index, data);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Get___VOID__I4__U4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType index = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    CLR_RT_HeapBlock_Array *array = stack.Arg3().DereferenceArray();
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        NF_RunTime_ISR_DataBuffer_Get(dataBuffer, index, data);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataBuffer_Clear___VOID__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_RunTime_ISR_DataBuffer_Clear(dataBuffer);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_GetMemorySize___U4__U4__U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_SharedDataOffsetType capacity = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg1());
    NF_Runtime_ISR_HeapOffsetType elementSize = ARG_AS_HEAPOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_SharedDataOffsetType result = NF_RunTime_ISR_DataRingBuffer_GetMemorySize(capacity, elementSize);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Initialize___VOID__I4__U4__U2( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType capacity = ARG_AS_SHAREDDATAOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_HeapOffsetType elementSize = ARG_AS_HEAPOFFSETTYPE(stack.Arg3());

    NF_RunTime_ISR_DataRingBuffer_Initialize(dataBuffer, capacity, elementSize);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Capacity___U4__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_SharedDataOffsetType capacity = NF_RunTime_ISR_DataRingBuffer_Capacity(dataBuffer);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(capacity);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_IsEmpty___BOOLEAN__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    bool isEmpty = NF_RunTime_ISR_DataRingBuffer_IsEmpty(dataBuffer);
    stack.SetResult_Boolean(isEmpty);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Push___BOOLEAN__I4__SZARRAY_U1__BOOLEAN( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    bool allowOverwrite = stack.Arg3().NumericByRef().s4 != 0;
    CLR_RT_HeapBlock_Array *array = stack.Arg2().DereferenceArray();
    bool result = false;
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        result = NF_RunTime_ISR_DataRingBuffer_Push(dataBuffer, data, allowOverwrite);
    }
    stack.SetResult_Boolean(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Peek___BOOLEAN__I4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    CLR_RT_HeapBlock_Array *array = stack.Arg2().DereferenceArray();
    bool result = false;
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        result = NF_RunTime_ISR_DataRingBuffer_PeekPop(dataBuffer, data, false);
    }
    stack.SetResult_Boolean(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Pop___BOOLEAN__I4__SZARRAY_U1( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    CLR_RT_HeapBlock_Array *array = stack.Arg2().DereferenceArray();
    bool result = false;
    if (array != NULL)
    {
        CLR_UINT8 *data = (CLR_UINT8 *)array->GetFirstElement();
        result = NF_RunTime_ISR_DataRingBuffer_PeekPop(dataBuffer, data, true);
    }
    stack.SetResult_Boolean(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DataRingBuffer_Clear___VOID__I4( CLR_RT_StackFrame &stack )
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_RunTime_ISR_DataRingBuffer_Clear(dataBuffer);

    NANOCLR_NOCLEANUP_NOLABEL();
}
