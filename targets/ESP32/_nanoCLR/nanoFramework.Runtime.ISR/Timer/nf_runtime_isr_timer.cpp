//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nf_runtime_isr_timer.h"

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
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_TimerInterruptBase::GetInterruptGeneratorMemorySize___U4,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_TimerInterruptBase::EnableInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_TimerInterruptBase::GetDataBusMemorySize___U4,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_TimerInterruptBase::InitialiseDataBus___VOID__I4,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Timer =
{
    "nanoFramework.Runtime.ISR.Timer",
    0xDB2786D0,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
