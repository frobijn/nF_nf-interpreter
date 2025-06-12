//-----------------------------------------------------------------------------
//
//                   ** WARNING! **
//    This file was generated automatically by a tool.
//    Re-running the tool will overwrite this file.
//    You should copy this file to a custom location
//    before adding any customization in the copy to
//    prevent loss of your changes when the tool is
//    re-run.
//
//-----------------------------------------------------------------------------

#include "NF_Runtime_ISR.h"
#include "nf_runtime_isr_core.h"

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::GetRequiredInterruptQueueSize___U2(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___memoryConfiguration]);

    CLR_UINT16 result = (configuration == nullptr ? 0 : configuration->RequiredInterruptQueueSize);
    stack.SetResult_U2(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::
    GetRequiredAfterInterruptStackSize___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___memoryConfiguration]);

    CLR_UINT32 result = (configuration == nullptr ? 0 : configuration->RequiredAfterInterruptStackSize);
    stack.SetResult_U4(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::GetRequiredManagedTaskStackSize___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    CLR_RT_HeapBlock *pThis = stack.This();
    NF_Runtime_ISR_Configuration *configuration = (NF_Runtime_ISR_Configuration *)ARG_AS_INTPTR(
        pThis[Library_nf_runtime_isr_core_nanoFramework_Runtime_ISR_DeviceConfiguration::FIELD___memoryConfiguration]);

    CLR_UINT32 result = (configuration == nullptr ? 0 : configuration->RequiredManagedTaskStackSize);
    stack.SetResult_U4(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}
