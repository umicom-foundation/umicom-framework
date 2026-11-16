# Umicom Framework
# File: examples/retrieval_sources/CMakeLists.txt
#
# PURPOSE:
# Build an independent offline retrieval-sources example from a selected installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT


cmake_minimum_required(VERSION 3.24)
project(UmicomRetrievalSourcesSdkExample LANGUAGES C)
set(UMICOM_SDK_ROOT "" CACHE PATH "Selected installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "Set UMICOM_SDK_ROOT to an absolute existing installed Framework SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ai_workspace
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
if(NOT COMMAND umicom_require_installed_sdk OR NOT COMMAND umicom_require_sdk_environment OR
   NOT COMMAND umicom_apply_application_branding)
    message(FATAL_ERROR "RetrievalSources example requires current installed SDK identity and branding helpers")
endif()
# Validate the selected package origin and ABI through its shared services.
# The consumer includes public headers and never compiles Framework sources.
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::ai_workspace)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-retrieval-sources main.c)
target_link_libraries(umicom-sdk-retrieval-sources PRIVATE Umicom::ai_workspace)
set_target_properties(umicom-sdk-retrieval-sources PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
umicom_apply_application_branding(TARGET umicom-sdk-retrieval-sources
    PRODUCT_NAME "Umicom RetrievalSources SDK Example" INTERNAL_NAME "umicom-sdk-retrieval-sources"
    APPLICATION_ID "org.umicom.sdk.retrieval-sources" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.retrieval-sources.observations COMMAND "$<TARGET_FILE:umicom-sdk-retrieval-sources>")
    set_tests_properties(sdk.retrieval-sources.observations PROPERTIES
        TIMEOUT 20 LABELS "sdk;ai-workspace;retrieval;example")
endif()
