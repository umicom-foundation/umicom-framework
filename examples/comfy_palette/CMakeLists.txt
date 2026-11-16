# Umicom Framework
# File: examples/comfy_palette/CMakeLists.txt
#
# PURPOSE:
# Build an independent owned ComfyUI node selection example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomComfyPaletteSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS comfy_protocol
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "ComfyPalette example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::comfy_protocol)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-comfy-palette main.c)
target_link_libraries(umicom-sdk-comfy-palette PRIVATE Umicom::comfy_protocol)
set_target_properties(umicom-sdk-comfy-palette PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-palette
    PRODUCT_NAME "Umicom ComfyPalette SDK Example" INTERNAL_NAME "umicom-sdk-comfy-palette"
    APPLICATION_ID "org.umicom.sdk.comfy-palette" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.comfy-palette.known-false COMMAND "$<TARGET_FILE:umicom-sdk-comfy-palette>")
    set_tests_properties(sdk.comfy-palette.known-false PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai;comfy-palette;example")
endif()
