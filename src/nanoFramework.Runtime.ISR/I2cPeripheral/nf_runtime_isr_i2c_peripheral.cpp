#include "nf_runtime_isr_i2c_peripheral.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral::DisposeNativeDataBus___VOID__I4,
    NULL,
    Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral::InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDeviceI2cI2cSlaveDevice,
    Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral__OnDataReceivedInterrupt::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral__OnDataReceivedInterrupt::Disable___VOID__I4,
    NULL,
    Library_nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral__OnDataReceivedInterrupt::Enable___VOID__I4__nanoFrameworkRuntimeISROnInterruptHandlers__SystemDeviceI2cI2cSlaveDevice,
    NULL,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_I2c_Peripheral =
{
    "nanoFramework.Runtime.ISR.I2c.Peripheral",
    0xADB1CC68,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
