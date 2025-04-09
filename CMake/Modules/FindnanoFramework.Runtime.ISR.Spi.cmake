#
# Copyright (c) .NET Foundation and Contributors
# See LICENSE file in the project root for full license information.
#

# native code directory
set(BASE_PATH_FOR_THIS_MODULE ${BASE_PATH_FOR_CLASS_LIBRARIES_MODULES}/nanoFramework.Runtime.ISR/Spi)


# set include directories
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Core)
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/CLR/Include)
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/HAL/Include)
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/PAL/Include)
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${BASE_PATH_FOR_THIS_MODULE})
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.ISR/Core)
list(APPEND nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.ISR/Spi)

# source files
set(nanoFramework.Runtime.ISR.Spi_SRCS

    nf_runtime_isr_spi_native.cpp
    nf_runtime_isr_spi.cpp
    nf_runtime_isr_spi_nanoFramework_Runtime_ISR_SpiDevice.cpp
)

foreach(SRC_FILE ${nanoFramework.Runtime.ISR.Spi_SRCS})

    set(nanoFramework.Runtime.ISR.Spi_SRC_FILE SRC_FILE-NOTFOUND)

    find_file(nanoFramework.Runtime.ISR.Spi_SRC_FILE ${SRC_FILE}
        PATHS
	        ${BASE_PATH_FOR_THIS_MODULE}
	        ${TARGET_BASE_LOCATION}
            ${PROJECT_SOURCE_DIR}/src/nanoFramework.Runtime.ISR/Spi

	    CMAKE_FIND_ROOT_PATH_BOTH
    )

    if (BUILD_VERBOSE)
        message("${SRC_FILE} >> ${nanoFramework.Runtime.ISR.Spi_SRC_FILE}")
    endif()

    list(APPEND nanoFramework.Runtime.ISR.Spi_SOURCES ${nanoFramework.Runtime.ISR.Spi_SRC_FILE})

endforeach()

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(nanoFramework.Runtime.ISR.Spi DEFAULT_MSG nanoFramework.Runtime.ISR.Spi_INCLUDE_DIRS nanoFramework.Runtime.ISR.Spi_SOURCES)
