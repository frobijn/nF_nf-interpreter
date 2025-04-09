#include "nf_runtime_isr_adc.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_runtime_isr_adc_nanoFramework_Runtime_ISR_AdcChannel::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_adc_nanoFramework_Runtime_ISR_AdcChannel::DisposeNativeDataBus___VOID__I4,
    NULL,
    Library_nf_runtime_isr_adc_nanoFramework_Runtime_ISR_AdcChannel::InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDeviceAdcAdcChannel,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Adc =
{
    "nanoFramework.Runtime.ISR.Adc",
    0x9BFC1804,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
