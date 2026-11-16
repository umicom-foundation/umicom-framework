#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/budgeted_buffer/CMakeLists.txt
#
# PURPOSE:
#   Build a bounded C allocation example against one explicit installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
project(umicom_budgeted_buffer LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS base
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
if(NOT DEFINED UmicomFramework_CONFIG OR NOT EXISTS "${UmicomFramework_CONFIG}")
    message(FATAL_ERROR "The selected Framework configuration is not an existing file")
endif()
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _sdk_inside)
if(NOT _sdk_inside)
    message(FATAL_ERROR "The selected Framework package is outside the requested prefix")
endif()
if(NOT COMMAND umicom_require_sdk_environment)
    message(FATAL_ERROR "Reinstall Framework with its SDK environment metadata")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::base)
add_executable(umicom-budgeted-buffer main.c)
set_target_properties(umicom-budgeted-buffer PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom-budgeted-buffer PRIVATE Umicom::base)
if(MSVC)
    target_compile_options(umicom-budgeted-buffer PRIVATE /W4)
else()
    target_compile_options(umicom-budgeted-buffer PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
umicom_apply_application_branding(TARGET umicom-budgeted-buffer
    PRODUCT_NAME "Umicom Bounded Buffer" INTERNAL_NAME "umicom-budgeted-buffer"
    APPLICATION_ID "org.umicom.sdk.memory.buffer"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.budgeted_buffer COMMAND umicom-budgeted-buffer)
    add_test(NAME sdk.budgeted_buffer.refusal COMMAND umicom-budgeted-buffer --reject-only)
    set_tests_properties(sdk.budgeted_buffer sdk.budgeted_buffer.refusal PROPERTIES TIMEOUT 10)
endif()
