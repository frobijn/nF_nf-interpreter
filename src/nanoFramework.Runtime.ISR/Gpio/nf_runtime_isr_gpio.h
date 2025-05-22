//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#ifndef NF_RUNTIME_ISR_GPIO_H
#define NF_RUNTIME_ISR_GPIO_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin
{
    static const int FIELD___gpioPin = 2;

    NANOCLR_NATIVE_DECLARE(GetNativeDataBusMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(InitialiseNativeDataBus___VOID__I4__BOOLEAN);
    NANOCLR_NATIVE_DECLARE(GetNativeInterruptGeneratorMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(EnableNativeInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers);
    NANOCLR_NATIVE_DECLARE(DisableNativeInterruptGenerator___VOID__I4);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Gpio;

#endif // NF_RUNTIME_ISR_GPIO_H