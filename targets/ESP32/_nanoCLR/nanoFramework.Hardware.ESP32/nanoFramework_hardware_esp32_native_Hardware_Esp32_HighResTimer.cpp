//
// Copyright (c) .NET Foundation and Contributors
// Portions Copyright (c) Microsoft Corporation.  All rights reserved.
// See LICENSE file in the project root for full license information.
//

#include "nanoFramework_hardware_esp32_native.h"
#include "CLR_IncludedAPI.h"
#ifdef API_nanoFramework_Runtime_ISR_Timer
#include "nf_runtime_isr_timer_esp32_highrestimer.h"
#endif

#define MAX_HRTIMERS 10

#ifdef API_nanoFramework_Runtime_ISR_Timer

typedef struct __nfpack EspTimer
{
    esp_timer_handle_t Timer;
    NF_Runtime_ISR_InterruptHandler *InterruptHandler;
} EspTimer;

EspTimer hrtimers[MAX_HRTIMERS] = {};

#define ESP32_TIMER_HANDLE(index) hrtimers[index].Timer

#else
esp_timer_handle_t hrtimers[MAX_HRTIMERS] = {};

#define ESP32_TIMER_HANDLE(index) hrtimers[index]

#endif

static int FindNextTimerIndex()
{
    for (int index = 0; index < MAX_HRTIMERS; index++)
    {
        if (ESP32_TIMER_HANDLE(index) == 0)
        {
#ifdef API_nanoFramework_Runtime_ISR_Timer
            hrtimers[index].InterruptHandler = nullptr;
#endif
            return index;
        }
    }

    return -1;
}

static void HRtimer_callback(void *arg)
{
#ifdef API_nanoFramework_Runtime_ISR_Timer
    if (hrtimers[(int)arg].InterruptHandler != nullptr)
    {
        NF_RunTime_ISR_HandleInterrupt(hrtimers[(int)arg].InterruptHandler, 0);
        return;
    }
#endif

    esp_timer_handle_t timer_handle = ESP32_TIMER_HANDLE((int)arg);
    PostManagedEvent(EVENT_HIGH_RESOLUTION_TIMER, HighResTimerEventType_TimerExpired, 0, (uint32_t)timer_handle);
}

#ifdef API_nanoFramework_Runtime_ISR_Timer
void NF_RunTime_ISR_InitialiseHighResTimer(
    CLR_RT_HeapBlock *timer,
    NF_Runtime_ISR_InterruptHandler *interruptHandlerData)
{
    esp_timer_handle_t handle = NF_RunTime_ISR_GetTimerHandle(timer);
    for (int index = 0; index < MAX_HRTIMERS; index++)
    {
        if (ESP32_TIMER_HANDLE(index) == handle)
        {
            hrtimers[index].InterruptHandler = interruptHandlerData;
            break;
        }
    }
}

esp_timer_handle_t NF_RunTime_ISR_GetTimerHandle(CLR_RT_HeapBlock *timer)
{
    if (timer == nullptr)
    {
        return 0;
    }
    return (esp_timer_handle_t)timer
        [Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::FIELD___timerHandle]
            .NumericByRefConst()
            .s4;
}
#endif

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::
    NativeEspTimerCreate___I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        int index = FindNextTimerIndex();
        if (index < 0)
            NANOCLR_SET_AND_LEAVE(CLR_E_TOO_MANY_OPEN_HANDLES);

        esp_timer_handle_t out_handle;
        esp_timer_create_args_t create_args = {};
        create_args.callback = &HRtimer_callback;
        create_args.arg = (void *)index;

        esp_err_t err = esp_timer_create(&create_args, &out_handle);
        if (err != ESP_OK)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_INVALID_PARAMETER);
        }
        ESP32_TIMER_HANDLE(index) = out_handle;
        stack.SetResult_I4((int32_t)out_handle);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::
    NativeEspTimerDispose___VOID(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[FIELD___timerHandle].NumericByRefConst().s4;
        esp_timer_delete(timer);

        for (int index = 0; index < MAX_HRTIMERS; index++)
        {
            if (ESP32_TIMER_HANDLE(index) == timer)
            {
                ESP32_TIMER_HANDLE(index) = 0;
                break;
            }
        }
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::NativeStop___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);
        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[FIELD___timerHandle].NumericByRefConst().s4;
        esp_timer_stop(timer);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::
    NativeStartOneShot___VOID__U8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[FIELD___timerHandle].NumericByRefConst().s4;
        uint64_t timeout_us = (uint64_t)stack.Arg1().NumericByRef().u8;

        esp_timer_stop(timer);
        esp_timer_start_once(timer, timeout_us);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::
    NativeStartPeriodic___VOID__U8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        if (pThis[FIELD___disposedValue].NumericByRef().u1 != 0)
        {
            NANOCLR_SET_AND_LEAVE(CLR_E_OBJECT_DISPOSED);
        }

        esp_timer_handle_t timer = (esp_timer_handle_t)pThis[FIELD___timerHandle].NumericByRefConst().s4;
        uint64_t period_us = (uint64_t)stack.Arg1().NumericByRef().u8;

        esp_timer_stop(timer);
        esp_timer_start_periodic(timer, period_us);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nanoFramework_hardware_esp32_native_nanoFramework_Hardware_Esp32_HighResTimer::
    NativeGetCurrent___STATIC__U8(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        uint64_t timer_val = esp_timer_get_time();
        stack.SetResult_U8(timer_val);
    }
    NANOCLR_NOCLEANUP_NOLABEL();
}
