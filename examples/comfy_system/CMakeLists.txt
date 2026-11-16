# Umicom Framework
# File: examples/comfy_system/CMakeLists.txt
#
# PURPOSE:
#   Build an independent native system-observation example from an explicitly selected installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

cmake_minimum_required(VERSION 3.24)
project(UmicomComfySystemSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS comfy_protocol
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "System observation example requires installed SDK identity and branding helpers")
endif()
# Reuse installed ABI/resource admission; never compile Framework source here.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::comfy_protocol)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-comfy-system main.c)
target_link_libraries(umicom-sdk-comfy-system PRIVATE Umicom::comfy_protocol)
set_target_properties(umicom-sdk-comfy-system PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-system
    PRODUCT_NAME "Umicom System Observation SDK Example" INTERNAL_NAME "umicom-sdk-comfy-system"
    APPLICATION_ID "org.umicom.sdk.comfy-system" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-system.observations COMMAND "$<TARGET_FILE:umicom-sdk-comfy-system>")
    set_tests_properties(sdk.comfy-system.observations PROPERTIES
        TIMEOUT 20 LABELS "sdk;comfy;system;example")
endif()


# Portable UTF-8 arguments and shared command fixtures come from this same SDK.
target_link_libraries(umicom-sdk-comfy-system PRIVATE Umicom::platform)
if(BUILD_TESTING)
    if(NOT COMMAND umicom_add_comfy_system_command_tests)
        message(FATAL_ERROR "Use an SDK with the captured system command qualification helper")
    endif()
    umicom_add_comfy_system_command_tests(umicom-sdk-comfy-system sdk.comfy-system.files)
endif()
