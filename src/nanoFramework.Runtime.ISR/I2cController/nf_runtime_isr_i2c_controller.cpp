#include "nf_runtime_isr_i2c_controller.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_runtime_isr_i2c_controller_nanoFramework_Runtime_ISR_I2cController::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_i2c_controller_nanoFramework_Runtime_ISR_I2cController::DisposeNativeDataBus___VOID__I4,
    NULL,
    Library_nf_runtime_isr_i2c_controller_nanoFramework_Runtime_ISR_I2cController::InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDeviceI2cI2cDevice,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_I2c_Controller =
{
    "nanoFramework.Runtime.ISR.I2c.Controller",
    0xA7C8F0BD,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
