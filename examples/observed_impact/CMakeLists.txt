#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/observed_impact/CMakeLists.txt
#
# PURPOSE:
#   Build the observation example from one explicit installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomObservedImpactExample LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply the installed SDK's absolute directory as UMICOM_EXPECTED_SDK_PREFIX")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS distribution
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
    message(FATAL_ERROR "Reinstall Framework with its environment metadata helpers")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::distribution)
add_executable(umicom-observed-impact main.c)
target_link_libraries(umicom-observed-impact PRIVATE Umicom::distribution)
set_target_properties(umicom-observed-impact PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-observed-impact PRIVATE /W4)
else()
    target_compile_options(umicom-observed-impact PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(scenario IN ITEMS same source first retry union unknown-retry history-pass history-failed history-unknown-tools saved-pass saved-failed saved-unknown-tools restored-pass restored-failed restored-edited)
        add_test(NAME sdk.observed_impact.${scenario} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:umicom-observed-impact>"
            "-DSCENARIO=${scenario}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/evidence/${scenario}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check.cmake")
        set_tests_properties(sdk.observed_impact.${scenario} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
