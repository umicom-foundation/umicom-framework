# Umicom Framework
# File: examples/comfy_queue/CMakeLists.txt
#
# PURPOSE:
# Build an independent copied ComfyUI queue observation example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomComfyQueueSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS comfy_protocol
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "ComfyQueue example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::comfy_protocol)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-comfy-queue main.c)
target_link_libraries(umicom-sdk-comfy-queue PRIVATE Umicom::comfy_protocol Umicom::platform)
set_target_properties(umicom-sdk-comfy-queue PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-queue
    PRODUCT_NAME "Umicom ComfyQueue SDK Example" INTERNAL_NAME "umicom-sdk-comfy-queue"
    APPLICATION_ID "org.umicom.sdk.comfy-queue" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-queue.observations COMMAND "$<TARGET_FILE:umicom-sdk-comfy-queue>")
    set_tests_properties(sdk.comfy-queue.observations PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai;comfy-queue;example")
endif()


if(BUILD_TESTING)
    if(NOT COMMAND umicom_add_comfy_queue_command_tests)
        message(FATAL_ERROR "Use an SDK with the captured queue command qualification helper")
    endif()
    umicom_add_comfy_queue_command_tests(umicom-sdk-comfy-queue sdk.comfy-queue.files)
endif()
