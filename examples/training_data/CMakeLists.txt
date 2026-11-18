#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/training_data/CMakeLists.txt
#
# PURPOSE:
#   Build a native data-preparation consumer from a selected installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.25)
project(UmicomTrainingDataExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute installed Framework SDK prefix")
endif()
# Preserve the earlier AI-only consumer lookup. The file example also uses
# Platform Unicode argument ownership, so request both documented components.
if(FALSE)
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ai
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ai platform
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "Use a matching SDK with identity and branding helpers")
endif()
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::ai)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-training-data main.c)
target_link_libraries(umicom-sdk-training-data PRIVATE Umicom::ai)
set_target_properties(umicom-sdk-training-data PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-training-data
    PRODUCT_NAME "Umicom Training Data Example" INTERNAL_NAME "umicom-sdk-training-data"
    APPLICATION_ID "org.umicom.sdk.training-data" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.training-data.records COMMAND umicom-sdk-training-data)
    set_tests_properties(sdk.training-data.records PROPERTIES TIMEOUT 20 LABELS "sdk;ai;training;example")
endif()

# A second thin consumer inspects paired context geometry without training.
add_executable(umicom-sdk-training-context context.c)
target_link_libraries(umicom-sdk-training-context PRIVATE Umicom::ai)
set_target_properties(umicom-sdk-training-context PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-training-context
    PRODUCT_NAME "Umicom Training Context Example" INTERNAL_NAME "umicom-sdk-training-context"
    APPLICATION_ID "org.umicom.sdk.training-context" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
if(BUILD_TESTING)
    add_test(NAME sdk.training-data.context COMMAND umicom-sdk-training-context)
    set_tests_properties(sdk.training-data.context PROPERTIES TIMEOUT 20 LABELS "sdk;ai;training;context")
endif()

# A file-selected consumer keeps OS argument handling at the shared boundary.
add_executable(umicom-sdk-training-stream stream.c)
target_link_libraries(umicom-sdk-training-stream PRIVATE Umicom::ai Umicom::platform)
set_target_properties(umicom-sdk-training-stream PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES
    C_EXTENSIONS NO RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-training-stream
    PRODUCT_NAME "Umicom Training Stream Example" INTERNAL_NAME "umicom-sdk-training-stream"
    APPLICATION_ID "org.umicom.sdk.training-stream" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
