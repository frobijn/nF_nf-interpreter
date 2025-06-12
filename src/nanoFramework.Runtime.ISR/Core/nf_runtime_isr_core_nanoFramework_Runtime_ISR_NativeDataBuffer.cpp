//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"
#include "nf_runtime_isr_core_memory.h"

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::ElementCount___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___lock]);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    NF_Runtime_ISR_MemoryOffsetType count = NF_Runtime_ISR_DataBuffer_Count(dataBuffer);
    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    SET_RESULT_AS_MEMORYOFFSETTYPE(count);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::Add___VOID__OBJECT__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___lock]);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    HRESULT hr2 = S_OK;
    CLR_UINT8 *memory = NF_Runtime_ISR_DataBuffer_Add(dataBuffer, nullptr);
    if (memory != nullptr)
    {
        hr2 = ReadWriteManagedData(WriteManagedData, stack.Arg1(), stack.Arg2(), memory);
    }

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::Set___VOID__U4__OBJECT__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___lock]);
    NF_Runtime_ISR_MemoryOffsetType index = ARG_AS_MEMORYOFFSETTYPE(stack.Arg1());

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    HRESULT hr2 = S_OK;
    CLR_UINT8 *memory = NF_Runtime_ISR_DataBuffer_Set(dataBuffer, index, nullptr);
    if (memory != nullptr)
    {
        hr2 = ReadWriteManagedData(WriteManagedData, stack.Arg2(), stack.Arg3(), memory);
    }

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::Get___VOID__U4__OBJECT__SZARRAY_U1(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___lock]);
    NF_Runtime_ISR_MemoryOffsetType index = ARG_AS_MEMORYOFFSETTYPE(stack.Arg1());

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    HRESULT hr2 = S_OK;
    CLR_UINT8 *memory = NF_Runtime_ISR_DataBuffer_Get(dataBuffer, index, nullptr);
    if (memory != nullptr)
    {
        hr2 = ReadWriteManagedData(ReadManagedData, stack.Arg2(), stack.Arg3(), memory);
    }

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::Clear___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___memory]);
    void *lock =
        ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer::FIELD___lock]);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    NF_Runtime_ISR_DataBuffer_Clear(dataBuffer);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_NOCLEANUP_NOLABEL();
}
