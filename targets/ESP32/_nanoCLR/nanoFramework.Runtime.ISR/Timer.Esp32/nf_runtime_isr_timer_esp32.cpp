//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nf_runtime_isr_timer_esp32.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::GetInterruptGeneratorMemorySize___U4,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::EnableInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::GetDataBusMemorySize___U4,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::InitialiseDataBus___VOID__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Timer_Esp32 =
{
    "nanoFramework.Runtime.ISR.Timer.Esp32",
    0x1AEF5A6F,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
