//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#include "nf_runtime_isr_gpio.h"

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
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::GetNativeDataBusMemorySize___U4,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::InitialiseNativeDataBus___VOID__I4__BOOLEAN,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::GetNativeInterruptGeneratorMemorySize___U4,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::EnableNativeInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::DisableNativeInterruptGenerator___VOID__I4,
    NULL,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Gpio =
{
    "nanoFramework.Runtime.ISR.Gpio",
    0x2763AD0F,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
