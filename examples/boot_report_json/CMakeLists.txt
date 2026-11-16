# Umicom Framework
# File: examples/boot_report_json/CMakeLists.txt
#
# PURPOSE:
# Build an independent captured boot-report comparison example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomBootReportJsonSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS boot_report_tools
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "BootReportJson example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::boot_report_tools)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-boot-report-json main.c)
target_link_libraries(umicom-sdk-boot-report-json PRIVATE Umicom::boot_report_tools)
set_target_properties(umicom-sdk-boot-report-json PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-boot-report-json
    PRODUCT_NAME "Umicom BootReportJson SDK Example" INTERNAL_NAME "umicom-sdk-boot-report-json"
    APPLICATION_ID "org.umicom.sdk.boot-report-json" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.boot-report-json.comparison COMMAND "$<TARGET_FILE:umicom-sdk-boot-report-json>")
    set_tests_properties(sdk.boot-report-json.known-false PROPERTIES
        TIMEOUT 20 LABELS "sdk;os;boot-report-json;example")
endif()
