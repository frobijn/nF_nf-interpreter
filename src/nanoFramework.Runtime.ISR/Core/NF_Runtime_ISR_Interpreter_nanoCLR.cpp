//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "NF_Runtime_ISR.h"

void NF_RunTime_ISR_InitialiseManagedActivation (NF_Runtime_ISR_ManagedActivation &managedActivation, CLR_RT_HeapBlock *managedActivationArgument)
{
    managedActivation.OnInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___onInterruptMemory]);
    managedActivation.AfterInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___afterInterruptMemory]);
    managedActivation.ServiceRoutineMemory = (CLR_UINT8 *)ARG_AS_INTPTR(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___activationMemory]);
    managedActivation.ServiceRoutineOffset = ARG_AS_SHAREDDATAOFFSETTYPE(managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___serviceRoutineOffset]);
    managedActivation.ServiceManagerId = managedActivationArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnManagedActivation::FIELD___serviceRoutineOffset].NumericByRef().s4;

    //managedActivation.RaiseManagedEvent = RaiseManagedEvent;
}

void NF_RunTime_ISR_InitialiseInterruptHandler (NF_Runtime_ISR_InterruptHandler &interruptHandler, CLR_RT_HeapBlock *onInterruptHandlerArgument)
{
    interruptHandler.OnInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___onInterruptMemory]);
    interruptHandler.OnInterruptSetArgumentsOffset = ARG_AS_SHAREDDATAOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___onInterruptSetArgumentsOffset]);
    interruptHandler.OnInterruptOffset = ARG_AS_SHAREDDATAOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___onInterruptOffset]);
    interruptHandler.AfterInterruptMemory = (CLR_UINT8 *)ARG_AS_INTPTR(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___afterInterruptMemory]);
    interruptHandler.AfterInterruptSetArgumentsOffset = ARG_AS_SHAREDDATAOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___afterInterruptSetArgumentsOffset]);
    interruptHandler.AfterInterruptOffset = ARG_AS_SHAREDDATAOFFSETTYPE(onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___afterInterruptOffset]);
    interruptHandler.ServiceManagerId = onInterruptHandlerArgument[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_OnInterruptHandlers::FIELD___managerId].NumericByRef().s4;

    //interruptHandler.RaiseManagedEvent = RaiseManagedEvent;
}



