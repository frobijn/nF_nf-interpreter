//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#ifndef NF_RUNTIME_ISR_TIMER_ESP32_H
#define NF_RUNTIME_ISR_TIMER_ESP32_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase
{
    static const int FIELD___singleShot = 1;
    static const int FIELD___timer = 2;
    static const int FIELD___initialInterval = 3;

    NANOCLR_NATIVE_DECLARE(GetInterruptGeneratorMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(EnableInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers);
    NANOCLR_NATIVE_DECLARE(GetDataBusMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(InitialiseDataBus___VOID__I4);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Timer_Esp32;

#endif // NF_RUNTIME_ISR_TIMER_ESP32_H
