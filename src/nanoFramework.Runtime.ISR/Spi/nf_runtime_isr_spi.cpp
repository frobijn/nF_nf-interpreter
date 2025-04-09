#include "nf_runtime_isr_spi.h"

// clang-format off

static const CLR_RT_MethodHandler method_lookup[] =
{
    NULL,
    Library_nf_runtime_isr_spi_nanoFramework_Runtime_ISR_SpiDevice::GetNativeMemorySize___U4,
    NULL,
    Library_nf_runtime_isr_spi_nanoFramework_Runtime_ISR_SpiDevice::DisposeNativeDataBus___VOID__I4,
    NULL,
    Library_nf_runtime_isr_spi_nanoFramework_Runtime_ISR_SpiDevice::InitialiseNativeDataBus___VOID__I4__BOOLEAN__SystemDeviceSpiSpiDevice,
    NULL,
};

const CLR_RT_NativeAssemblyData g_CLR_AssemblyNative_nanoFramework_Runtime_ISR_Spi =
{
    "nanoFramework.Runtime.ISR.Spi",
    0xA5194F29,
    method_lookup,
    { 1, 0, 0, 0 }
};

// clang-format on
