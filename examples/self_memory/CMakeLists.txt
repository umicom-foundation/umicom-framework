#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/self_memory/CMakeLists.txt
#
# PURPOSE:
#   Build a self-memory observation example against one explicit installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
project(umicom_self_memory LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS desktop_system
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
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::desktop_system)
add_executable(umicom-memory-observation main.c)
set_target_properties(umicom-memory-observation PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(umicom-memory-observation PRIVATE Umicom::desktop_system)
if(MSVC)
    target_compile_options(umicom-memory-observation PRIVATE /W4)
else()
    target_compile_options(umicom-memory-observation PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
umicom_apply_application_branding(TARGET umicom-memory-observation
    PRODUCT_NAME "Umicom Memory Observation" INTERNAL_NAME "umicom-memory-observation"
    APPLICATION_ID "org.umicom.sdk.memory.observation"
    VERSION_MAJOR 1 VERSION_MINOR 0 VERSION_PATCH 0)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.self_memory COMMAND umicom-memory-observation)
    set_tests_properties(sdk.self_memory PROPERTIES TIMEOUT 20)
endif()
