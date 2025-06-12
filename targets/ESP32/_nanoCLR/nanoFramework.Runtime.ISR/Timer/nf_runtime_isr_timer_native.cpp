//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_runtime_isr_timer.h"
#include "nf_runtime_isr_timer_esp32_highrestimer.h"

//----------------------------------------------------------------------
//
// Interrupt generator
//
//----------------------------------------------------------------------

HRESULT Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::GetInterruptGeneratorMemorySize___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType result = sizeof(NF_Runtime_ISR_InterruptHandler);
    SET_RESULT_AS_MEMORYOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::
    EnableNativeInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        NF_Runtime_ISR_InterruptHandler *interruptData = (NF_Runtime_ISR_InterruptHandler *)ARG_AS_INTPTR(stack.Arg1());
        CLR_RT_HeapBlock *interruptHandlers = stack.Arg2().Dereference();
        NF_Runtime_ISR_InitialiseInterruptHandler(*interruptData, interruptHandlers);

        NF_Runtime_ISR_InitialiseHighResTimer(
            pThis[Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::FIELD___timer].Dereference(),
            interruptData);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::DisableNativeInterruptGenerator___VOID(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        NF_Runtime_ISR_DisableHighResTimer(
            pThis[Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::FIELD___timer].Dereference());
    }
    NANOCLR_NOCLEANUP();
}

//----------------------------------------------------------------------
//
// Data bus
//
//----------------------------------------------------------------------

typedef struct __nfpack TimerAsDataBusData
{
    NF_Runtime_ISR_DataBus Methods;
    esp_timer_handle_t Timer;
} TimerAsDataBusData;

static void TimerExecute(
    struct NF_Runtime_ISR_DataBus *dataBus,
    CLR_UINT8 methodIndex,
    CLR_UINT8 dataCount,
    void **dataPtr,
    NF_Runtime_ISR_MemoryOffsetType *dataSize,
    void *result,
    NF_Runtime_ISR_MemoryOffsetType resultSize)
{
    if (methodIndex == 0)
    {
        *(CLR_UINT64 *)*dataPtr = esp_timer_get_time();
    }
    else
    {
        esp_timer_handle_t timer = ((TimerAsDataBusData *)dataBus)->Timer;
        esp_timer_stop(timer);
        if (methodIndex != 12)
        {
            CLR_UINT64 interval = *(CLR_UINT64 *)*dataPtr;
            if (interval != 0)
            {
                if (methodIndex == 10)
                {
                    esp_timer_start_once(timer, interval);
                }
                else
                {
                    esp_timer_start_periodic(timer, interval);
                }
            }
        }
    }
}

HRESULT Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::GetDataBusMemorySize___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType result = sizeof(TimerAsDataBusData);
    SET_RESULT_AS_MEMORYOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::InitialiseDataBus___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    TimerAsDataBusData *dataBus = (TimerAsDataBusData *)ARG_AS_INTPTR(stack.Arg1());

    CLR_RT_HeapBlock *pThis = stack.This();
    dataBus->Timer = NF_Runtime_ISR_GetTimerHandle(
        pThis[Library_nf_runtime_isr_timer_nanoFramework_Runtime_ISR_Timer::FIELD___timer].Dereference());
    dataBus->Methods.Execute = TimerExecute;

    NANOCLR_NOCLEANUP_NOLABEL();
}