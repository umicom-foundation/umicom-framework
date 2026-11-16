# Umicom Framework
# File: examples/media_generation/CMakeLists.txt
#
# PURPOSE:
# Build the offline video recovery example as an independent consumer of an installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomMediaSdkExamples LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS pixverse
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "Media examples require installed SDK identity and branding helpers")
endif()
# The imported video component carries the shared provider/data dependencies.
# Origin and environment checks come from the selected SDK, not copied logic.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::pixverse)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-video-recovery pixverse_task_recovery.c)
target_link_libraries(umicom-sdk-video-recovery PRIVATE Umicom::pixverse)
set_target_properties(umicom-sdk-video-recovery PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-video-recovery
    PRODUCT_NAME "Umicom Video Recovery Example" INTERNAL_NAME "umicom-sdk-video-recovery"
    APPLICATION_ID "org.umicom.sdk.video-recovery" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.media_examples.task_recovery COMMAND "$<TARGET_FILE:umicom-sdk-video-recovery>")
    set_tests_properties(sdk.media_examples.task_recovery PROPERTIES
        TIMEOUT 20 LABELS "sdk;media;provider-connections;example")
endif()
