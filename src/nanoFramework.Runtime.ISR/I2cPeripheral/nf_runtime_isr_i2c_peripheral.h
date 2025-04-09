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

#ifndef NF_RUNTIME_ISR_I2C_PERIPHERAL_H
#define NF_RUNTIME_ISR_I2C_PERIPHERAL_H

#include <nanoCLR_Interop.h>
#include <nanoCLR_Runtime.h>
#include <nanoPackStruct.h>
#include <corlib_native.h>

struct Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral
{
    static const int FIELD__<peripheral>P = 1;

    NANOCLR_NATIVE_DECLARE(GetNativeMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(DisposeNativeDataBus___VOID__I4);
    NANOCLR_NATIVE_DECLARE(InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDeviceI2cI2cSlaveDevice);

    //--//
};

struct Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral__OnDataReceivedInterrupt
{
    static const int FIELD__<peripheral>P = 1;

    NANOCLR_NATIVE_DECLARE(GetNativeMemorySize___U4);
    NANOCLR_NATIVE_DECLARE(Disable___VOID__I4);
    NANOCLR_NATIVE_DECLARE(Enable___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers__SystemDeviceI2cI2cSlaveDevice);

    //--//
};

extern const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_I2c_Peripheral;

#endif // NF_RUNTIME_ISR_I2C_PERIPHERAL_H
