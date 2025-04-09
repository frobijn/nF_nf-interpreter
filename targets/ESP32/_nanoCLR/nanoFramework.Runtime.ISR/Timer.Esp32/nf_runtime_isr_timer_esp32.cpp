#include "nf_runtime_isr_timer_esp32.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    NULL,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_OnHighResTimerInterrupt::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_OnHighResTimerInterrupt::Disable___VOID__I4,
    NULL,
    Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_OnHighResTimerInterrupt::Enable___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers__nanoFrameworkHardwareEsp32HighResTimer,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Timer_Esp32 =
{
    "nanoFramework.Runtime.ISR.Timer.Esp32",
    0x1AFBAD87,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
