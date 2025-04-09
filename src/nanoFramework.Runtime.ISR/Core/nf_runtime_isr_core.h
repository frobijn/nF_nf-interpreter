//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_RUNTIME_ISR_CORE_H
#define NF_RUNTIME_ISR_CORE_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack InterpreterMemoryType
{
    InterpreterMemoryType_ISR = 0,
    InterpreterMemoryType_Task = 1,
    InterpreterMemoryType_ManagedActivation = 2,
} InterpreterMemoryType;

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation
{
    static const int FIELD___onInterruptMemory = 1;
    static const int FIELD___afterInterruptMemory = 2;
    static const int FIELD___activationMemory = 3;
    static const int FIELD___serviceRoutineOffset = 4;
    static const int FIELD___managerId = 5;

    //--//
};

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager
{
    NANOCLR_NATIVE_DECLARE(DoAllocateMemory___BOOLEAN__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__BYREF_I4);
    NANOCLR_NATIVE_DECLARE(DoAssignMemory___VOID__I4__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DoAssignOffset___VOID__I4__U4__U4);
    NANOCLR_NATIVE_DECLARE(DoIsOffsetAssigned___BOOLEAN__I4__U4);
    NANOCLR_NATIVE_DECLARE(DoGetMemoryPointer___I4__I4__U4);
    NANOCLR_NATIVE_DECLARE(DoReadMemory___VOID__I4__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DoReleaseMemory___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__I4);
    NANOCLR_NATIVE_DECLARE(DoStartServiceRoutine___VOID__nanoFrameworkRuntimeISROnManagedActivation);
    NANOCLR_NATIVE_DECLARE(DataBuffer_GetMemorySize___U4__U4__U2);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Initialize___VOID__I4__U4__U2);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Capacity___U4__I4);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Count___U4__I4);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Add___VOID__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Insert___VOID__I4__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Get___VOID__I4__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Clear___VOID__I4);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_GetMemorySize___U4__U4__U2);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Initialize___VOID__I4__U4__U2);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Capacity___U4__I4);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_IsEmpty___BOOLEAN__I4);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Push___BOOLEAN__I4__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Peek___BOOLEAN__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Pop___BOOLEAN__I4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Clear___VOID__I4);

    //--//
};

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers
{
    static const int FIELD___onInterruptMemory = 1;
    static const int FIELD___onInterruptSetArgumentsOffset = 2;
    static const int FIELD___onInterruptOffset = 3;
    static const int FIELD___afterInterruptMemory = 4;
    static const int FIELD___afterInterruptSetArgumentsOffset = 5;
    static const int FIELD___afterInterruptOffset = 6;
    static const int FIELD___managerId = 7;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Core;

#endif // NF_RUNTIME_ISR_CORE_H
