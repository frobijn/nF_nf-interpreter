//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"
#include "nf_runtime_isr_core_memory.h"

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::BufferIsEmpty___BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___lock]);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    bool isEmpty = NF_Runtime_ISR_DataRingBuffer_IsEmpty(dataBuffer);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    stack.SetResult_Boolean(isEmpty);
    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::
    Push___BOOLEAN__OBJECT__SZARRAY_U1__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___lock]);
    bool allowOverwrite = stack.Arg3().NumericByRef().s4 != 0;
    bool success;

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    HRESULT hr2 = S_OK;
    CLR_UINT8 *memory = NF_Runtime_ISR_DataRingBuffer_Push(dataBuffer, nullptr, allowOverwrite, success);
    if (memory != nullptr)
    {
        hr2 = ReadWriteManagedData(WriteManagedData, stack.Arg1(), stack.Arg2(), memory);
    }

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    stack.SetResult_Boolean(success);
    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::Peek___BOOLEAN__OBJECT__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___lock]);
    bool isAvailable;

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    HRESULT hr2 = S_OK;
    CLR_UINT8 *memory = NF_Runtime_ISR_DataRingBuffer_PeekPop(dataBuffer, nullptr, false, isAvailable);
    if (memory != nullptr)
    {
        hr2 = ReadWriteManagedData(ReadManagedData, stack.Arg1(), stack.Arg2(), memory);
    }

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    stack.SetResult_Boolean(isAvailable);
    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::Pop___BOOLEAN__OBJECT__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___lock]);
    bool isAvailable;

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    HRESULT hr2 = S_OK;
    CLR_UINT8 *memory = NF_Runtime_ISR_DataRingBuffer_PeekPop(dataBuffer, nullptr, true, isAvailable);
    if (memory != nullptr)
    {
        hr2 = ReadWriteManagedData(ReadManagedData, stack.Arg1(), stack.Arg2(), memory);
    }

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    stack.SetResult_Boolean(isAvailable);
    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::Clear___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer::FIELD___lock]);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    NF_Runtime_ISR_DataRingBuffer_Clear(dataBuffer);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}
