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
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::GetInterruptGeneratorMemorySize___U4,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::EnableNativeInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::DisableNativeInterruptGenerator___VOID,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::GetDataBusMemorySize___U4,
    Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::InitialiseDataBus___VOID__I4,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Timer =
{
    "nanoFramework.Runtime.ISR.Timer",
    0x20473F4F,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
