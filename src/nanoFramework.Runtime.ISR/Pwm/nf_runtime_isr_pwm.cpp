#include "nf_runtime_isr_pwm.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_runtime_isr_pwm_nanoFramework_Runtime_ISR_PwmChannel::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_pwm_nanoFramework_Runtime_ISR_PwmChannel::DisposeNativeDataBus___VOID__I4,
    NULL,
    Library_nf_runtime_isr_pwm_nanoFramework_Runtime_ISR_PwmChannel::InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDevicePwmPwmChannel,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Pwm =
{
    "nanoFramework.Runtime.ISR.Pwm",
    0xFA69EC17,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
