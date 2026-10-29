#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/reading_document/CMakeLists.txt
#
# PURPOSE:
#   Build a standalone reading-model example against one explicit installed SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomReadingExample LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS browser
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _inside)
if(NOT _inside)
    message(FATAL_ERROR "The selected package is outside the requested SDK prefix")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::browser)
add_executable(umicom-reading-example main.c)
target_link_libraries(umicom-reading-example PRIVATE Umicom::browser)
set_target_properties(umicom-reading-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-reading-example PRIVATE /W4)
else()
    target_compile_options(umicom-reading-example PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.browser.reading_document COMMAND umicom-reading-example)
    set_tests_properties(sdk.browser.reading_document PROPERTIES TIMEOUT 30)
endif()
