//
// Copyright (c) .NET Foundation and Contributors
// See LICENSE file in the project root for full license information.
//

#include "nf_runtime_isr_timer_esp32.h"
#include "nf_runtime_isr_timer_esp32_highrestimer.h"

//----------------------------------------------------------------------
//
// Interrupt generator
//
//----------------------------------------------------------------------

HRESULT Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::
    GetInterruptGeneratorMemorySize___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_SharedDataOffsetType result = sizeof(NF_Runtime_ISR_InterruptHandler);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::
    EnableInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        NF_Runtime_ISR_InterruptHandler *interruptData = (NF_Runtime_ISR_InterruptHandler *)ARG_AS_INTPTR(stack.Arg1());
        CLR_RT_HeapBlock *interruptHandlers = stack.Arg2().Dereference();
        NF_RunTime_ISR_InitialiseInterruptHandler(*interruptData, interruptHandlers);

        NF_RunTime_ISR_InitialiseHighResTimer(
            pThis[Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::FIELD___timer]
                .Dereference(),
            interruptData);
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

static void DataBusGetCurrentTime(
    struct NF_Runtime_ISR_DataBus *dataBus,
    CLR_UINT8 *dataPtr,
    NF_Runtime_ISR_HeapOffsetType dataSize,
    void *result)
{
    *(CLR_UINT64 *)dataPtr = esp_timer_get_time();
}

static void DataBusSetPeriod(
    struct NF_Runtime_ISR_DataBus *dataBus,
    CLR_UINT8 *dataPtr,
    NF_Runtime_ISR_HeapOffsetType dataSize,
    void *result)
{
    esp_timer_handle_t timer = ((TimerAsDataBusData *)dataBus)->Timer;

    esp_timer_stop(timer);
    esp_timer_start_periodic(timer, *(CLR_UINT64 *)dataPtr);
}

static void DataBusSetOneShot(
    struct NF_Runtime_ISR_DataBus *dataBus,
    CLR_UINT8 *dataPtr,
    NF_Runtime_ISR_HeapOffsetType dataSize,
    void *result)
{
    esp_timer_handle_t timer = ((TimerAsDataBusData *)dataBus)->Timer;

    esp_timer_stop(timer);
    esp_timer_start_once(timer, *(CLR_UINT64 *)dataPtr);
}

HRESULT Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::
    GetDataBusMemorySize___U4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_SharedDataOffsetType result = sizeof(TimerAsDataBusData);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::
    InitialiseDataBus___VOID__I4(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        TimerAsDataBusData *dataBus = (TimerAsDataBusData *)ARG_AS_INTPTR(stack.Arg1());

        dataBus->Timer = NF_RunTime_ISR_GetTimerHandle(
            pThis[Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::FIELD___timer]
                .Dereference());

        dataBus->Methods.Read = DataBusGetCurrentTime;
        if (pThis[Library_nf_runtime_isr_timer_esp32_nanoFramework_Runtime_ISR_Esp32_TimerInterruptBase::
                      FIELD___singleShot]
                .NumericByRef()
                .s4 == 0)
        {
            dataBus->Methods.Write = DataBusSetPeriod;
        }
        else
        {
            dataBus->Methods.Write = DataBusSetOneShot;
        }
    }
    NANOCLR_NOCLEANUP();
}