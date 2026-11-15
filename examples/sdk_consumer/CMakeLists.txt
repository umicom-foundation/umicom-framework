#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/sdk_consumer/CMakeLists.txt
#
# PURPOSE:
#   Configure the sdk consumer build rules without duplicating product logic.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

# Minimal installed-SDK consumer used by integrators and release validation.
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkConsumer LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Explicit selection is useful for extracted archives. Keep the original
# CMAKE_PREFIX_PATH discovery when no root is supplied for existing integrators.
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(UMICOM_SDK_ROOT)
    if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
        message(FATAL_ERROR "UMICOM_SDK_ROOT must be an absolute existing SDK prefix")
    endif()
    find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS sdk delivery
        PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
    umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::sdk Umicom::delivery)
    umicom_require_sdk_environment(2)
else()
    find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS sdk delivery)
endif()
add_executable(umicom-sdk-consumer main.c)
target_link_libraries(umicom-sdk-consumer PRIVATE Umicom::sdk Umicom::delivery)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.consumer.export_plan COMMAND umicom-sdk-consumer)
endif()
