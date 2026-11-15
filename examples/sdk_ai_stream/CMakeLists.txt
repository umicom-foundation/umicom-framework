#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/sdk_ai_stream/CMakeLists.txt
#
# PURPOSE:
# Build a branded independent AI stream client from an explicitly selected installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
project(UmicomSdkAiStream VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(UMICOM_SDK_ROOT "" CACHE PATH "Installed Framework SDK prefix")
if(NOT IS_ABSOLUTE "${UMICOM_SDK_ROOT}" OR NOT IS_DIRECTORY "${UMICOM_SDK_ROOT}")
    message(FATAL_ERROR "UMICOM_SDK_ROOT must name an absolute existing installed SDK prefix")
endif()
find_package(UmicomFramework 0.9 CONFIG REQUIRED COMPONENTS ai
    PATHS "${UMICOM_SDK_ROOT}" NO_DEFAULT_PATH)
umicom_require_installed_sdk("${UMICOM_SDK_ROOT}" Umicom::ai)
umicom_require_sdk_environment(2)
add_executable(umicom-sdk-ai-stream main.c)
target_link_libraries(umicom-sdk-ai-stream PRIVATE Umicom::ai)
umicom_apply_application_branding(TARGET umicom-sdk-ai-stream
    PRODUCT_NAME "Umicom AI Stream SDK Example" INTERNAL_NAME "umicom-sdk-ai-stream"
    APPLICATION_ID "org.umicom.sdk.ai.stream" VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)

# The independent installed consumer is also an offline qualification target.
enable_testing()
add_test(NAME sdk.ai_stream.owned_ranges COMMAND umicom-sdk-ai-stream)
set_tests_properties(sdk.ai_stream.owned_ranges PROPERTIES TIMEOUT 30 LABELS "sdk;ai;stream;offline")
