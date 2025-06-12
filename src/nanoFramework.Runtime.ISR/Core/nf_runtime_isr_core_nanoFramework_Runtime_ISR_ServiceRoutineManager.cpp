//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"
#include "nf_runtime_isr_core_memory.h"

/*
Define ENABLE_ISR_MANAGEDREADWRITE_DIAGNOSTICS to report extra information about type incompatibilities
encountered in the conversion of managed data to/from ISR memory.
*/

#ifdef ENABLE_ISR_MANAGEDREADWRITE_DIAGNOSTICS
static void ISR_Diagnostics_IncorrectArrayType(CLR_UINT32 managedArray, CLR_UINT32 elementType)
{
    char temporaryStringBuffer[45];
    int realStringSize = snprintf(
        temporaryStringBuffer,
        sizeof(temporaryStringBuffer),
        "CLR_DataType array: %d, ISR memory: %d\r\n",
        managedArray,
        elementType);
    CLR_EE_DBG_EVENT_BROADCAST(
        CLR_DBG_Commands_c_Monitor_Message,
        realStringSize,
        temporaryStringBuffer,
        WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
}

static void ISR_Diagnostics_IncorrectObjectType(CLR_UINT32 dataType)
{
    char temporaryStringBuffer[40];
    int realStringSize =
        snprintf(temporaryStringBuffer, sizeof(temporaryStringBuffer), "CLR_DataType managed object: %d\r\n", dataType);
    CLR_EE_DBG_EVENT_BROADCAST(
        CLR_DBG_Commands_c_Monitor_Message,
        realStringSize,
        temporaryStringBuffer,
        WP_Flags_c_NonCritical | WP_Flags_c_NoCaching);
}
#endif

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
    stack.SetResult_Boolean(false);
    {
        InterpreterMemoryType memoryType = (InterpreterMemoryType)stack.Arg1().NumericByRef().s4;
        void *memory = nullptr;

        // Also allocate memory for the device/platform configuration if that has not yet been done.
        CLR_RT_HeapBlock *pThis = stack.This();
        NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___memoryConfigurationAndState]);
        if (configuration == nullptr)
        {
            CLR_RT_HeapBlock *pConfiguration =
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___deviceConfiguration]
                    .Dereference();
            bool isISRMemorySpecial =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___isISRMemorySpecial]
                    .NumericByRef()
                    .s4 != 0;

            NF_Runtime_ISR_MemoryOffsetType configurationSize = NF_Runtime_ISR_GetConfigurationMemorySize();
            NANOCLR_CHECK_HRESULT(
                NF_Runtime_ISR_AllocateMemory(memoryType, configurationSize, isISRMemorySpecial, &memory));
            configuration = (NF_Runtime_ISR_Configuration *)memory;

            configuration->IsISRMemorySpecial = isISRMemorySpecial;
            configuration->InterruptQueueSize =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___interruptQueueSize]
                    .NumericByRef()
                    .u2;
            configuration->AfterInterruptStackSize =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___afterInterruptStackSize]
                    .NumericByRef()
                    .u4;
            configuration->AfterInterruptPriority =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___afterInterruptPriority]
                    .NumericByRef()
                    .u4;
            configuration->ManagedTaskStackSize =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___managedTaskStackSize]
                    .NumericByRef()
                    .u4;
            configuration->ManagedTaskPriority =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___managedTaskPriority]
                    .NumericByRef()
                    .u4;
            configuration->MonitorResourceUse =
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___monitorResourceUse]
                    .NumericByRef()
                    .s4 != 0;
            SET_AS_INTPTR(
                pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                                   FIELD___memoryConfiguration],
                configuration);
            SET_AS_INTPTR(
                pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                          FIELD___memoryConfigurationAndState],
                configuration);
        }

        NF_Runtime_ISR_MemoryOffsetType size = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
        NANOCLR_CHECK_HRESULT(
            NF_Runtime_ISR_AllocateMemory(memoryType, size, configuration->IsISRMemorySpecial, &memory));

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

    CLR_RT_HeapBlock *pThis = stack.This();
    NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                  FIELD___memoryConfigurationAndState]);

    InterpreterMemoryType configurationMemory = InterpreterMemoryType::InterpreterMemoryType_ManagedActivation;

    void *memory = ARG_AS_INTPTR(pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                                           FIELD___memoryManagedActivation]);
    if (memory != nullptr)
    {
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_ReleaseMemory(
            configuration,
            InterpreterMemoryType::InterpreterMemoryType_ManagedActivation,
            memory));
        SET_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___memoryManagedActivation],
            0);
    }

    memory = ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryTask]);
    if (memory != nullptr)
    {
        NANOCLR_CHECK_HRESULT(
            NF_Runtime_ISR_ReleaseMemory(configuration, InterpreterMemoryType::InterpreterMemoryType_Task, memory));
        SET_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryTask],
            0);
        configurationMemory = InterpreterMemoryType::InterpreterMemoryType_Task;
    }

    memory = ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryISR]);
    if (memory != nullptr)
    {
        NANOCLR_CHECK_HRESULT(
            NF_Runtime_ISR_ReleaseMemory(configuration, InterpreterMemoryType::InterpreterMemoryType_ISR, memory));
        SET_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::FIELD___memoryISR],
            0);
        configurationMemory = InterpreterMemoryType::InterpreterMemoryType_ISR;
    }

    if (configuration != nullptr)
    {
        NF_Runtime_ISR_ReleaseRTOSTaskData(configuration);
        NF_Runtime_ISR_ReleaseManagedActivationTaskData(configuration);

        SET_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___memoryConfigurationAndState],
            0);

        CLR_RT_HeapBlock *pConfiguration =
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___deviceConfiguration]
                .Dereference();
        SET_AS_INTPTR(
            pConfiguration[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
                               FIELD___memoryConfiguration],
            0);
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_ReleaseMemory(configuration, configurationMemory, configuration));
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::EnableRTOSTask___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___memoryConfigurationAndState]);
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_AllocateRTOSTaskData(configuration));
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_EnableRTOSTask(configuration));
        CLR_RT_HeapBlock *pConfiguration =
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___deviceConfiguration]
                .Dereference();
        pConfiguration
            [Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___interruptQueueSize]
                .SetInteger(configuration->InterruptQueueSize);
        pConfiguration
            [Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___afterInterruptStackSize]
                .SetInteger(configuration->AfterInterruptStackSize);
        pConfiguration
            [Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___afterInterruptPriority]
                .SetInteger(configuration->AfterInterruptPriority);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::DisableRTOSTask___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                  FIELD___memoryConfigurationAndState]);
    NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_DisableRTOSTask(configuration));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DoStartServiceRoutine___VOID__nanoFrameworkRuntimeISROnManagedActivation__BOOLEAN(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *managedActivationArgument = stack.Arg1().Dereference();
    NF_Runtime_ISR_ManagedActivation managedActivation;
    NF_Runtime_ISR_InitialiseManagedActivation(managedActivation, managedActivationArgument);

    bool inTask = (bool)stack.Arg2().NumericByRef().u1;
    if (inTask)
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___memoryConfigurationAndState]);
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_AllocateManagedActivationTaskData(configuration));
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_StartManagedActivationTask(configuration, &managedActivation));

        CLR_RT_HeapBlock *pConfiguration =
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___deviceConfiguration]
                .Dereference();
        pConfiguration
            [Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___managedTaskStackSize]
                .SetInteger(configuration->ManagedTaskStackSize);
        pConfiguration
            [Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___managedTaskPriority]
                .SetInteger(configuration->ManagedTaskPriority);
    }
    else
    {
        NF_Runtime_ISR_RunServiceRoutine(&managedActivation);
    }

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    StopManagedActivatedServiceRoutine___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
            pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
                      FIELD___memoryConfigurationAndState]);
        NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_StopManagedActivationTask(configuration));
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_GetMemorySize___U4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType result = NF_Runtime_ISR_DataBuffer_GetMemorySize(capacity, elementSize);
    SET_RESULT_AS_MEMORYOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

//----------------------------------------------------------------------
//
// Read/write managed data from/to memory
//
//----------------------------------------------------------------------

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    ReadManagedData___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__I4__OBJECT__SZARRAY_U1(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    memory += offset;
    void *lock = ARG_AS_INTPTR(stack.Arg3());

    HRESULT hr2 = S_OK;
    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    hr2 = ReadWriteManagedData(ReadManagedData, stack.Arg4(), stack.Arg5(), memory);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    WriteManagedData___VOID__nanoFrameworkRuntimeISRInterpreterMemoryType__U4__I4__OBJECT__SZARRAY_U1(
        CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *memory = (CLR_UINT8 *)GetMemoryPointer(stack);
    NF_Runtime_ISR_MemoryOffsetType offset = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    memory += offset;
    void *lock = ARG_AS_INTPTR(stack.Arg3());

    HRESULT hr2 = S_OK;
    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockEnterScope(lock);
    }

    hr2 = ReadWriteManagedData(WriteManagedData, stack.Arg4(), stack.Arg5(), memory);

    if (lock != nullptr)
    {
        NF_Runtime_ISR_LockExitScope(lock);
    }

    NANOCLR_SET_AND_LEAVE(hr2);
    NANOCLR_NOCLEANUP();
}

//----------------------------------------------------------------------
//
// Locks
//
//----------------------------------------------------------------------

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::Lock_GetMemorySize___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType result = NF_Runtime_ISR_LockGetMemorySize();
    SET_RESULT_AS_MEMORYOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::Lock_Initialize___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    void *lock = ARG_AS_INTPTR(stack.Arg1());
    NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_LockInitialise(lock));

    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::Lock_Dispose___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    void *lock = ARG_AS_INTPTR(stack.Arg1());
    NANOCLR_CHECK_HRESULT(NF_Runtime_ISR_LockRelease(lock));

    NANOCLR_NOCLEANUP();
}

//----------------------------------------------------------------------
//
// Data buffers: memory allocation/initialization
//
//----------------------------------------------------------------------

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataBuffer_Initialize___VOID__I4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_UINT8 *dataBuffer = (CLR_UINT8 *)ARG_AS_INTPTR(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg3());

    NF_Runtime_ISR_DataBuffer_Initialize(dataBuffer, capacity, elementSize);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_ServiceRoutineManager::
    DataRingBuffer_GetMemorySize___U4__U4__U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType capacity = ARG_AS_MEMORYOFFSETTYPE(stack.Arg1());
    NF_Runtime_ISR_MemoryOffsetType elementSize = ARG_AS_MEMORYOFFSETTYPE(stack.Arg2());
    NF_Runtime_ISR_MemoryOffsetType result = NF_Runtime_ISR_DataRingBuffer_GetMemorySize(capacity, elementSize);
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

    NF_Runtime_ISR_DataRingBuffer_Initialize(dataBuffer, capacity, elementSize);

    NANOCLR_NOCLEANUP_NOLABEL();
}
