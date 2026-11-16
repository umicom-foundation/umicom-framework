# Umicom Framework
# File: examples/ollama_pull/CMakeLists.txt
#
# PURPOSE:
# Build an independent offline model-pull progress example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomOllamaPullSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ai
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "OllamaPull example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::ai)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-ollama-pull main.c)
target_link_libraries(umicom-sdk-ollama-pull PRIVATE Umicom::ai)
set_target_properties(umicom-sdk-ollama-pull PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-ollama-pull
    PRODUCT_NAME "Umicom OllamaPull SDK Example" INTERNAL_NAME "umicom-sdk-ollama-pull"
    APPLICATION_ID "org.umicom.sdk.ollama-pull" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.ollama-pull.progress COMMAND "$<TARGET_FILE:umicom-sdk-ollama-pull>")
    # The former property referred to an unregistered test. Keep it for review;
    # properties now address the existing progress test by its exact name.
    if(FALSE)
    set_tests_properties(sdk.ollama-pull.observations PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai;ollama-pull;example")
    endif()
    set_tests_properties(sdk.ollama-pull.progress PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai;ollama-pull;example")
    if(NOT COMMAND umicom_add_pull_inspection_command_tests)
        message(FATAL_ERROR "This example requires the installed model-progress test helper")
    endif()
    umicom_add_pull_inspection_command_tests(umicom-sdk-ollama-pull sdk.ollama-pull.file)
endif()
