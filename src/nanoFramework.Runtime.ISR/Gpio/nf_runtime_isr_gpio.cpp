#include "nf_runtime_isr_gpio.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::DisposeNativeDataBus___VOID__I4,
    NULL,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDeviceGpioGpioPin,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin__OnValueChangedInterrupt::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin__OnValueChangedInterrupt::Disable___VOID__I4,
    NULL,
    Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin__OnValueChangedInterrupt::Enable___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers__SystemDeviceGpioGpioPin,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Gpio =
{
    "nanoFramework.Runtime.ISR.Gpio",
    0xE666A04F,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
