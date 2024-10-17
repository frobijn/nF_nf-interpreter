# Unique device ID

## Motivation

In a development/test environment several (hardware) nanoDevices may be connected to a computer at once. The devices may differ in make and model, not only in the MCU on board, but also in connected sensors, actuators, etc. In case the MCUs are of the same type, the name of the firmware/target is not enough to distinguish the devices. And the randomness of the names of the serial ports the devices are connected to (certainly from one computer to the other) makes it hard for a developer to recognise the device in the Device Explorer. Tools also cannot identify the device, unless the developer actively maintains a list of serial ports and the connected devices.

The devices will be used to deploy applications to for debugging via the Visual Studio extension, or for running unit tests. Identification of the device has to be done based on the properties exposed by the firmware, as the deployed application is overwritten each time. Most device types do have some kind of baked-in unique ID, but that is not exposed via the firmware.

At least for the reference/community targets, there are no other properties available that uniquely identify the device. There are provisions to expose a unique ID: the module serial number and system serial number. There is no mechanism to assign these numbers, the only way to use them is to create custom firmware. But custom firmware is a non-starter for a .NET developer attracted to the nanoFramework.

The proposed change is designed to expose the baked-in unique ID through the system serial number. Additional requirements are:

- The change should not mess up any existing custom firmware
- The change should be backward compatible: all existing software should still be working with the solution applied.

## Proposed solution

The status of the system serial number changes from "only supported in custom firmware" to "only supported in custom firmware and for device types that offer a unique device ID". And for the latter, the system serial number is opaque: no developer should try to parse the bytes of the serial number to get the unique device ID or other information.

The device types that have a unique ID:

- ESP32 has a baked in, unchangeable MAC address (6 bytes)
- STM32 and Silabs Giant Gecko have a unique device ID (4 bytes)
- Other (not yet supported) devices like RP2040 have a unique device ID of 8 bytes.

For other device types (SMT32) nothing changes.

The system serial number is an array of 16 bytes. It will be constructed as:

- The first *n* bytes are the the unique device ID. Having this first makes it easier to see the differences between serial numbers.
- The next *16-n* bytes are the (maximum) *16-n* characters of the target name that distinguish this device type from others (e.g., ESP32S3, STM32F769I). Its function is to make the unique device ID globally unique.

The system serial number is exposed as part of a structure that is passed to a function [GetHalSystemInfo](https://github.com/nanoframework/nf-interpreter/blob/main/src/HAL/nanoHAL_SystemInformation.cpp#L13). That structure is instantiated as a [wire protocol reply](https://github.com/nanoframework/nf-interpreter/blob/main/src/CLR/Debugger/Debugger.cpp#L1658). The `GetHalSystemInfo` method is not called anywhere else.

The `GetHalSystemInfo` method has no provision to assign the system serial number. There is some [code](https://github.com/nanoframework/nf-interpreter/blob/main/src/HAL/nanoHAL_SystemInformation.cpp#L42) but that is commented out. The definition if its provider, [g_ConfigurationSector](https://github.com/nanoframework/nf-interpreter/blob/main/src/PAL/Include/nanoPAL_Sockets.h#L472), is also commented out and references to its type `ConfigurationSector` can only be found in comments in the old win32 target.

The last [comment block](https://github.com/nanoframework/nf-interpreter/blob/main/src/HAL/nanoHAL_SystemInformation.cpp#L40) in `GetHalSystemInfo` is replaced by:
```
#ifdef HAS_UNIQUE_DEVICE_ID
    // A system serial number is derived from a unique device ID if that is available.
    // Replace this in custom firmware to implement a custom serial number scheme.
    CreateSerialNumberFromDeviceId(&systemInfo.m_OemSerialNumbers.system_serial_number, sizeof(systemInfo.m_OemSerialNumbers.system_serial_number));
#endif
```

The `HAS_UNIQUE_DEVICE_ID` is assigned only for device types that have a unique device ID in the `CMakePresets`. It is translated into a preprocessor define in the global `CMakeLists.txt` to a `-DHAS_UNIQUE_DEVICE_ID` compiler option. The `CreateSerialNumberFromDeviceId` function is placed in the `<targetname>_DeviceMapping.cpp` file. It looks like:
```
void CreateSerialNumberFromDeviceId(byte * serialNumber, size_t length)
{
    CopyDeviceIdToSerialNumber(serialNumber, length);
    hal_strncopy(&serialNumber[n], length-n, "ESP32S3", sizeof("ESP32S3"));
}
```
The `CopyDeviceIdToSerialNumber` can be located elsewhere in the device-specific implementation of the CLR.

# Custom firmware and backward compatibility

The proposed solution does not mess up custom firmware in the sense that:

- The new code will only be active in custom firmware if that is defined as a variant of an existing reference/community target, otherwise the `HAS_UNIQUE_DEVICE_ID` is not defined. Defining firmware as a variant implies that the developer accepts that the developer has to monitor changes to the `<targetname>_DeviceMapping.cpp` file, as the firmware includes a custom file that replaces that file. The build will fail because the `CreateSerialNumberFromDeviceId` is missing.

- If the firmware implements system serial numbers, it must contain a custom implementation of the `GetHalSystemInfo` method. If the custom branch is updated by merging the new *main* branch, this will lead to a merge conflict. Every developer knows that needs some attention. Even if auto-merge is performed, the new code will not be active or will lead to a build error (see previous point).

Either there is no change in serial number, or the developer will be notified that there is a change and has to take action. In no situation the serial number changes unnoticed from zeros to a non-zero value.

It is also backward compatible, as the serial numbers are only exposed in .NET via the wire protocol. Generic tools that use the `nf_debugger` code will not have any business logic using serial numbers as there are now meaningless. Tools that are expecting the serial numbers to be set must be designed for a specific product and those serial numbers will not be affected.
