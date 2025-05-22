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

#include "nf_runtime_isr_gpio.h"
#include "NF_Runtime_ISR.h"
#include "sys_dev_gpio_native.h"

//----------------------------------------------------------------------
//
// Data bus
//
//----------------------------------------------------------------------

typedef struct __nfpack GpioPinAsDataBusData
{
    NF_Runtime_ISR_DataBus Methods;
    CLR_RT_HeapBlock *gpioPin;
} GpioPinAsDataBusData;

static void ReadGpioPin(
    NF_Runtime_ISR_DataBus *dataBus,
    CLR_UINT8 *dataPtr,
    NF_Runtime_ISR_MemoryOffsetType dataSize,
    void *result)
{
    bool pinValue;
    Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Read(((GpioPinAsDataBusData *)dataBus)->gpioPin, pinValue);
    *dataPtr = pinValue ? GpioPinValue::GpioPinValue_High : GpioPinValue::GpioPinValue_Low;
}

static void WriteGpioPin(
    NF_Runtime_ISR_DataBus *dataBus,
    CLR_UINT8 *dataPtr,
    NF_Runtime_ISR_MemoryOffsetType dataSize,
    void *result)
{
    GpioPinValue pinValue =
        *dataPtr == GpioPinValue::GpioPinValue_Low ? GpioPinValue::GpioPinValue_Low : GpioPinValue::GpioPinValue_High;
    Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::Write(((GpioPinAsDataBusData *)dataBus)->gpioPin, pinValue);
}

HRESULT Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::GetNativeDataBusMemorySize___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType result = sizeof(GpioPinAsDataBusData);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::InitialiseNativeDataBus___VOID__I4__BOOLEAN(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        GpioPinAsDataBusData *dataBus = (GpioPinAsDataBusData *)ARG_AS_INTPTR(stack.Arg1());

        dataBus->Methods.Read = ReadGpioPin;
        dataBus->Methods.Write = WriteGpioPin;
        dataBus->gpioPin =
            pThis[Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::FIELD___gpioPin].Dereference();
        FAULT_ON_NULL(dataBus->gpioPin);
    }
    NANOCLR_NOCLEANUP();
}

//----------------------------------------------------------------------
//
// Interrupt generator
//
//----------------------------------------------------------------------

static void ISR_InterruptHandler(void *param, GpioPinValue pinValue)
{
    NF_RunTime_ISR_HandleInterrupt(
        (NF_Runtime_ISR_InterruptHandler *)param,
        pinValue == GpioPinValue::GpioPinValue_Low ? PinEventTypes::PinEventTypes_Falling
                                                   : PinEventTypes::PinEventTypes_Rising);
}

HRESULT Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::GetNativeInterruptGeneratorMemorySize___U4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();

    NF_Runtime_ISR_MemoryOffsetType result = sizeof(NF_Runtime_ISR_InterruptHandler);
    SET_RESULT_AS_SHAREDDATAOFFSETTYPE(result);

    NANOCLR_NOCLEANUP_NOLABEL();
}

HRESULT Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::
    EnableNativeInterruptGenerator___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers(CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        NF_Runtime_ISR_InterruptHandler *interruptData = (NF_Runtime_ISR_InterruptHandler *)ARG_AS_INTPTR(stack.Arg1());
        CLR_RT_HeapBlock *interruptHandlers = stack.Arg2().Dereference();
        NF_RunTime_ISR_InitialiseInterruptHandler(*interruptData, interruptHandlers);

        CLR_RT_HeapBlock *gpioPin =
            pThis[Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::FIELD___gpioPin].Dereference();
        FAULT_ON_NULL(gpioPin);

        GPIO_PIN pinNumber =
            (GPIO_PIN)gpioPin[Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::FIELD___pinNumber]
                .NumericByRefConst()
                .s4;

        CPU_GPIO_SetNativeISR(pinNumber, ISR_InterruptHandler, GPIO_INT_EDGE::GPIO_INT_EDGE_BOTH, interruptData);
    }
    NANOCLR_NOCLEANUP();
}

HRESULT Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::DisableNativeInterruptGenerator___VOID__I4(
    CLR_RT_StackFrame &stack)
{
    NANOCLR_HEADER();
    {
        CLR_RT_HeapBlock *pThis = stack.This();
        FAULT_ON_NULL(pThis);

        CLR_RT_HeapBlock *gpioPin =
            pThis[Library_nf_runtime_isr_gpio_nanoFramework_Runtime_ISR_GpioPin::FIELD___gpioPin].Dereference();
        FAULT_ON_NULL(gpioPin);

        GPIO_PIN pinNumber =
            (GPIO_PIN)gpioPin[Library_sys_dev_gpio_native_System_Device_Gpio_GpioPin::FIELD___pinNumber]
                .NumericByRefConst()
                .s4;
        CPU_GPIO_SetNativeISR(pinNumber, NULL, GPIO_INT_EDGE::GPIO_INT_NONE, NULL);
    }
    NANOCLR_NOCLEANUP();
}