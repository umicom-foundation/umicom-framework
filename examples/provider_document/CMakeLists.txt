# Umicom Framework
# File: examples/provider_document/CMakeLists.txt
#
# PURPOSE:
# Build an independent offline provider settings document example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomProviderDocumentSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS provider_connections
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "ProviderDocument example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::provider_connections)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-provider-document main.c)
target_link_libraries(umicom-sdk-provider-document PRIVATE Umicom::provider_connections Umicom::platform)
set_target_properties(umicom-sdk-provider-document PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-provider-document
    PRODUCT_NAME "Umicom ProviderDocument SDK Example" INTERNAL_NAME "umicom-sdk-provider-document"
    APPLICATION_ID "org.umicom.sdk.provider-document" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.provider-document.round-trip COMMAND "$<TARGET_FILE:umicom-sdk-provider-document>")
    set_tests_properties(sdk.provider-document.round-trip PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai;provider-document;example")
endif()


# Use the producer's same file fixtures after installation. Missing fixture
# sources are an incomplete SDK, not a reason to silently skip qualification.
if(BUILD_TESTING)
    set(_document_driver "${CMAKE_CURRENT_LIST_DIR}/check_document_file.cmake")
    if(NOT EXISTS "${_document_driver}")
        set(_document_driver "${CMAKE_CURRENT_LIST_DIR}/../../tests/provider_connections/check_document_file.cmake")
    endif()
    if(NOT EXISTS "${_document_driver}")
        message(FATAL_ERROR "Install the provider document example qualification driver from the same SDK")
    endif()
    foreach(_case valid unicode missing directory relative empty malformed unknown-version crlf within-limit over-limit help extra)
        add_test(NAME sdk.provider-document.file.${_case} COMMAND "${CMAKE_COMMAND}"
            "-DTEST_EXE=$<TARGET_FILE:umicom-sdk-provider-document>"
            "-DTEST_ROOT=${CMAKE_CURRENT_BINARY_DIR}/provider-document-files/${_case}"
            "-DTEST_CASE=${_case}" "-DTEST_SDK=TRUE"
            -P "${_document_driver}")
        set_tests_properties(sdk.provider-document.file.${_case} PROPERTIES
            TIMEOUT 35 LABELS "sdk;provider-document;native-file")
    endforeach()
endif()
