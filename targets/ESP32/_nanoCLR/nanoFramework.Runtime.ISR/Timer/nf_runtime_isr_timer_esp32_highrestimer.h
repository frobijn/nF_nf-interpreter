//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//
#ifndef NF_RUNTIME_ISR_TIMER_ESP32_HIGHRESTIMER_H
#define NF_RUNTIME_ISR_TIMER_ESP32_HIGHRESTIMER_H

#include "NF_Runtime_ISR.h"

extern void NF_RunTime_ISR_InitialiseHighResTimer(
    CLR_RT_HeapBlock *timer,
    NF_Runtime_ISR_InterruptHandler *interruptHandlerData);

extern void NF_RunTime_ISR_DisableHighResTimer(CLR_RT_HeapBlock *timer);

extern esp_timer_handle_t NF_RunTime_ISR_GetTimerHandle(CLR_RT_HeapBlock *timer);

#endif // NF_RUNTIME_ISR_TIMER_ESP32_HIGHRESTIMER_H
