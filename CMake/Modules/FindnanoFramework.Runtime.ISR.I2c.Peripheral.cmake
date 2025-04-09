#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Runtime.ISR.I2c.Peripheral)


# set include directories
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.ISR/Core)
list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.ISR/I2c.Peripheral)

# source files
set(nanoFramework.Runtime.ISR.I2c.Peripheral_SRCS

    nf_runtime_isr_i2c_peripheral_native.cpp
    nf_runtime_isr_i2c_peripheral.cpp
    nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral.cpp
    nf_runtime_isr_i2c_peripheral_nanoFramework_Runtime_ISR_I2cPeripheral__OnDataReceivedInterrupt.cpp
)

foreach(SRC_FILE ${nanoFramework.Runtime.ISR.I2c.Peripheral_SRCS})

    set(nanoFramework.Runtime.ISR.I2c.Peripheral_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Runtime.ISR.I2c.Peripheral_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.ISR/I2c.Peripheral

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Runtime.ISR.I2c.Peripheral_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Runtime.ISR.I2c.Peripheral_SOURCES ${nanoFramework.Runtime.ISR.I2c.Peripheral_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Runtime.ISR.I2c.Peripheral DEFAULT_MSG nanoFramework.Runtime.ISR.I2c.Peripheral_INCLUDE_DIRS nanoFramework.Runtime.ISR.I2c.Peripheral_SOURCES)
