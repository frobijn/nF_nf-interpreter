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

#ifndef NF_RUNTIME_ISR_PWM_H
#define NF_RUNTIME_ISR_PWM_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

typedef enum __nfpack PwmDutyCycle
{
    PwmDutyCycle_MinValue = 0,
    PwmDutyCycle_MaxValue = 10000,
    PwmDutyCycle_Stop = -1,
} PwmDutyCycle;

struct Library_nf_runtime_isr_pwm_nanoFramework_Runtime_ISR_PwmChannel
{
    NANOCLR_NATIVE_DECLARE(GetNativeMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(DisposeNativeDataBus___VOID__I4);
    NANOCLR_NATIVE_DECLARE(InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDevicePwmPwmChannel);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Pwm;

#endif // NF_RUNTIME_ISR_PWM_H
