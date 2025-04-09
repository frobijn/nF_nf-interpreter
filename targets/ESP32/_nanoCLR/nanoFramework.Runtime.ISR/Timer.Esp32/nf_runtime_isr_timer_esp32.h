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

#ifndef NF_RUNTIME_ISR_TIMER_ESP32_H
#define NF_RUNTIME_ISR_TIMER_ESP32_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_OnHighResTimerInterrupt
{
    NANOCLR_NATIVE_DECLARE(GetNativeMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(Disable___VOID__I4);
    NANOCLR_NATIVE_DECLARE(Enable___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers__nanoFrameworkHardwareEsp32HighResTimer);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Timer_Esp32;

#endif // NF_RUNTIME_ISR_TIMER_ESP32_H
