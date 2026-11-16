# Umicom Framework
# File: examples/comfy_inspection/CMakeLists.txt
#
# PURPOSE:
# Build an independent offline ComfyUI file inspection example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomComfyInspectionSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS comfy_protocol
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "ComfyInspection example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::comfy_protocol)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-comfy-inspection main.c)
target_link_libraries(umicom-sdk-comfy-inspection PRIVATE Umicom::comfy_protocol Umicom::platform)
set_target_properties(umicom-sdk-comfy-inspection PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-comfy-inspection
    PRODUCT_NAME "Umicom ComfyInspection SDK Example" INTERNAL_NAME "umicom-sdk-comfy-inspection"
    APPLICATION_ID "org.umicom.sdk.comfy-inspection" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    umicom_add_comfy_inspection_command_tests(umicom-sdk-comfy-inspection sdk.comfy-inspection.command)
    add_test(NAME sdk.comfy-inspection.help COMMAND "$<TARGET_FILE:umicom-sdk-comfy-inspection>" --help)
    add_test(NAME sdk.comfy-inspection.example COMMAND "$<TARGET_FILE:umicom-sdk-comfy-inspection>" inspect
        "${CMAKE_CURRENT_SOURCE_DIR}/connection.umiconnection"
        "${CMAKE_CURRENT_SOURCE_DIR}/workflow.json" "${CMAKE_CURRENT_SOURCE_DIR}/object-info.json")
    set_tests_properties(sdk.comfy-inspection.help sdk.comfy-inspection.example PROPERTIES TIMEOUT 20 LABELS "sdk;comfy;offline;example")

    # The installed command implementation also owns reference observations.
    # The bundled literal-only workflow intentionally produces an empty page.
    add_test(NAME sdk.comfy-inspection.references-empty COMMAND "$<TARGET_FILE:umicom-sdk-comfy-inspection>" references
        "${CMAKE_CURRENT_SOURCE_DIR}/connection.umiconnection"
        "${CMAKE_CURRENT_SOURCE_DIR}/workflow.json" "${CMAKE_CURRENT_SOURCE_DIR}/object-info.json" --limit 1)
    set_tests_properties(sdk.comfy-inspection.references-empty PROPERTIES TIMEOUT 20 LABELS "sdk;comfy;offline;example")
endif()
