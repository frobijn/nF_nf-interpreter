//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_RUNTIME_ISR_CORE_MEMORY_H
#define NF_RUNTIME_ISR_CORE_MEMORY_H

extern HRESULT ReadManagedData(
    CLR_RT_HeapBlock *instanceArg,
    CLR_UINT8 **dataType,
    bool asArrayElement,
    CLR_UINT8 **data);

extern HRESULT WriteManagedData(
    CLR_RT_HeapBlock *instanceArg,
    CLR_UINT8 **dataType,
    bool asArrayElement,
    CLR_UINT8 **data);

extern HRESULT ReadWriteManagedData(
    HRESULT (*readWrite)(CLR_RT_HeapBlock *instanceArg, CLR_UINT8 **dataType, bool asArrayElement, CLR_UINT8 **data),
    CLR_RT_HeapBlock &instanceArg,
    CLR_RT_HeapBlock &dataTypeArg,
    CLR_UINT8 *data);

#endif // NF_RUNTIME_ISR_CORE_MEMORY_H
