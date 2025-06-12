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

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration
{
    static const int FIELD___isISRMemorySpecial = 1;
    static const int FIELD___interruptQueueSize = 2;
    static const int FIELD___afterInterruptStackSize = 3;
    static const int FIELD___afterInterruptPriority = 4;
    static const int FIELD___managedTaskStackSize = 5;
    static const int FIELD___managedTaskPriority = 6;
    static const int FIELD___monitorResourceUse = 7;
    static const int FIELD___memoryConfiguration = 8;

    NANOCLR_NATIVE_DECLARE(GetRequiredInterruptQueueSize___U2);
    NANOCLR_NATIVE_DECLARE(GetRequiredAfterInterruptStackSize___U4);
    NANOCLR_NATIVE_DECLARE(GetRequiredManagedTaskStackSize___U4);

    //--//
};

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation
{
    static const int FIELD___onInterruptMemory = 1;
    static const int FIELD___afterInterruptMemory = 2;
    static const int FIELD___activationMemory = 3;
    static const int FIELD___serviceRoutineOffset = 4;
    static const int FIELD___configuration = 5;

    //--//
};

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager
{
    static const int FIELD___memoryISR = 1;
    static const int FIELD___memoryTask = 2;
    static const int FIELD___memoryManagedActivation = 3;
    static const int FIELD___memoryConfigurationAndState = 4;
    static const int FIELD___deviceConfiguration = 5;
    static const int FIELD___serviceRoutineManagerId = 6;

    NANOCLR_NATIVE_DECLARE(DoAllocateMemory___BOOLEAN__nanoFrameworkRuntimeISRInterpreterMemoryType__U4);
    NANOCLR_NATIVE_DECLARE(AssignMemory___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(AssignOffset___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__U4);
    NANOCLR_NATIVE_DECLARE(IsOffsetAssigned___BOOLEAN__nanoFrameworkRuntimeISRInterpreterMemoryType__U4);
    NANOCLR_NATIVE_DECLARE(GetMemoryPointer___I4__nanoFrameworkRuntimeISRInterpreterMemoryType__U4);
    NANOCLR_NATIVE_DECLARE(
        ReadManagedData___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__I4__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(
        WriteManagedData___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__I4__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(ReleaseMemory___VOID);
    NANOCLR_NATIVE_DECLARE(DoStartServiceRoutine___VOID__nanoFrameworkRuntimeISROnManagedActivation__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(StopManagedActivatedServiceRoutine___VOID);
    NANOCLR_NATIVE_DECLARE(EnableRTOSTask___VOID);
    NANOCLR_NATIVE_DECLARE(DisableRTOSTask___VOID);
    NANOCLR_NATIVE_DECLARE(Lock_GetMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(Lock_Initialize___VOID__I4);
    NANOCLR_NATIVE_DECLARE(Lock_Dispose___VOID__I4);
    NANOCLR_NATIVE_DECLARE(DataBuffer_GetMemorySize___U4__U4__U4);
    NANOCLR_NATIVE_DECLARE(DataBuffer_Initialize___VOID__I4__U4__U4);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_GetMemorySize___U4__U4__U4);
    NANOCLR_NATIVE_DECLARE(DataRingBuffer_Initialize___VOID__I4__U4__U4);

    //--//
};

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataBuffer
{
    static const int FIELD___memory = 1;
    static const int FIELD___lock = 2;

    NANOCLR_NATIVE_DECLARE(ElementCount___U4);
    NANOCLR_NATIVE_DECLARE(Add___VOID__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(Set___VOID__U4__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(Get___VOID__U4__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(Clear___VOID);

    //--//
};

struct Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_NativeDataRingBuffer
{
    static const int FIELD___memory = 1;
    static const int FIELD___lock = 2;

    NANOCLR_NATIVE_DECLARE(BufferIsEmpty___BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Push___BOOLEAN__OBJECT__SZARRAY_U1__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(Peek___BOOLEAN__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(Pop___BOOLEAN__OBJECT__SZARRAY_U1);
    NANOCLR_NATIVE_DECLARE(Clear___VOID);

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
    static const int FIELD___configuration = 7;

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Core;

#endif // NF_RUNTIME_ISR_CORE_H
