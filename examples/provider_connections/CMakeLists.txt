# Umicom Framework
# File: examples/provider_connections/CMakeLists.txt
#
# PURPOSE:
# Build the existing provider lessons against a selected installed SDK without a Framework source checkout.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomProviderSdkExamples LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS provider_connections
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "Provider examples require current installed SDK identity and branding helpers")
endif()
# Reuse the shared installed-origin traversal; never add a repository include
# directory or compile Framework implementation files into the consumer.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::provider_connections)
# This consumer requires ABI 2 independently of the selected producer metadata.
umicom_require_sdk_environment(2)
include(CTest)

function(umicom_provider_sdk_example target source product application test_name)
    add_executable("${target}" "${source}")
    target_link_libraries("${target}" PRIVATE Umicom::provider_connections)
    set_target_properties("${target}" PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    umicom_apply_application_branding(TARGET "${target}"
        PRODUCT_NAME "${product}" INTERNAL_NAME "${target}"
        APPLICATION_ID "${application}" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
    if(BUILD_TESTING)
        add_test(NAME "sdk.provider_examples.${test_name}" COMMAND "$<TARGET_FILE:${target}>")
        set_tests_properties("sdk.provider_examples.${test_name}" PROPERTIES
            TIMEOUT 20 LABELS "sdk;provider-connections;example")
    endif()
endfunction()

umicom_provider_sdk_example(umicom-sdk-provider-connections saved_connections.c
    "Umicom Provider Connections Example" "org.umicom.sdk.provider-connections" connections)
umicom_provider_sdk_example(umicom-sdk-provider-task task_receipt.c
    "Umicom Provider Task Example" "org.umicom.sdk.provider-task" receipt)
umicom_provider_sdk_example(umicom-sdk-provider-store stored_tasks.c
    "Umicom Provider Store Example" "org.umicom.sdk.provider-store" store)
