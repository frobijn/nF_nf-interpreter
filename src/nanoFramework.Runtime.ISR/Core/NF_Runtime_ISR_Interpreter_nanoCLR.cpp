//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"

static void RaiseManagedEvent(CLR_INT32 managerId, CLR_UINT32 eventArg)
{
    PostManagedEvent(EVENT_SERVICE_ROUTINE, 0, (uint16_t)managerId, (uint32_t)eventArg);
}

void NF_RunTime_ISR_InitialiseManagedActivation(
    NF_Runtime_ISR_ManagedActivation &managedActivation,
    CLR_RT_HeapBlock *managedActivationArgument)
{
    // clang-format off
    managedActivation.OnInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___onInterruptMemory]);
    managedActivation.AfterInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___afterInterruptMemory]);
    managedActivation.ServiceRoutineMemory = (CLR_UINT8 *)ARG_AS_INTPTR(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___activationMemory]);
    managedActivation.ServiceRoutineOffset = ARG_AS_MEMORYOFFSETTYPE(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___serviceRoutineOffset]);
    managedActivation.ServiceManagerId = managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___managerId].NumericByRef().s4;
    managedActivation.RaiseManagedEvent = RaiseManagedEvent;
    // clang-format on
}

void NF_RunTime_ISR_InitialiseInterruptHandler(
    NF_Runtime_ISR_InterruptHandler &interruptHandler,
    CLR_RT_HeapBlock *onInterruptHandlerArgument)
{
    // clang-format off
    interruptHandler.OnInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___onInterruptMemory]);
    interruptHandler.OnInterruptSetArgumentsOffset = ARG_AS_MEMORYOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___onInterruptSetArgumentsOffset]);
    interruptHandler.OnInterruptOffset = ARG_AS_MEMORYOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___onInterruptOffset]);
    interruptHandler.AfterInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___afterInterruptMemory]);
    interruptHandler.AfterInterruptSetArgumentsOffset = ARG_AS_MEMORYOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___afterInterruptSetArgumentsOffset]);
    interruptHandler.AfterInterruptOffset = ARG_AS_MEMORYOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___afterInterruptOffset]);
    interruptHandler.TaskMemory = ARG_AS_INTPTR(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___taskMemory]);
    interruptHandler.ServiceManagerId = onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___managerId].NumericByRef().s4;
    interruptHandler.RaiseManagedEvent = RaiseManagedEvent;
    // clang-format on
}
