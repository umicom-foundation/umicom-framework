#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/test_evidence/CMakeLists.txt
#
# PURPOSE:
#   Build a test evidence example against one explicit installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomTestEvidence LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS test_platform
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
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::test_platform)
add_executable(umicom-test-evidence main.c)
target_link_libraries(umicom-test-evidence PRIVATE Umicom::test_platform)
set_target_properties(umicom-test-evidence PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-test-evidence PRIVATE /W4)
else()
    target_compile_options(umicom-test-evidence PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()


include(CTest)
if(BUILD_TESTING)
    foreach(_case IN ITEMS result short-output coverage overflow benchmark)
        add_test(NAME sdk.test_evidence.${_case} COMMAND umicom-test-evidence ${_case})
        set_tests_properties(sdk.test_evidence.${_case} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
