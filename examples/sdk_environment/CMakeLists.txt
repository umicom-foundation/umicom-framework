#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/sdk_environment/CMakeLists.txt
#
# PURPOSE:
#   Check one installed SDK through an independent C23 consumer.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomSdkEnvironmentExample LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS base
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
# A previously cached package directory can override normal search preferences.
# Check the actual configuration file as well as the imported library paths.
if(NOT DEFINED UmicomFramework_CONFIG OR NOT EXISTS "${UmicomFramework_CONFIG}")
    message(FATAL_ERROR "The selected SDK has no identifiable package configuration")
endif()
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _sdk_selected_here)
if(NOT _sdk_selected_here)
    message(FATAL_ERROR "CMake selected a package outside UMICOM_EXPECTED_SDK_PREFIX; use a fresh consumer build directory")
endif()
if(NOT COMMAND umicom_require_sdk_environment)
    message(FATAL_ERROR "This example requires an SDK with declared environment metadata; reinstall the current Framework")
endif()
# The application declares the ABI it was written against. This requirement is
# intentionally independent of the producer's value, so a mismatch is visible.
set(UMICOM_REQUIRED_ABI 2)
umicom_require_sdk_environment("${UMICOM_REQUIRED_ABI}")
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::base)
add_executable(umicom-sdk-environment-example main.c)
set_target_properties(umicom-sdk-environment-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_compile_definitions(umicom-sdk-environment-example PRIVATE
    UMICOM_REQUIRED_ABI=${UMICOM_REQUIRED_ABI}
    UMICOM_PACKAGE_POINTER_BYTES=${UMICOM_FRAMEWORK_TARGET_POINTER_BYTES})
target_link_libraries(umicom-sdk-environment-example PRIVATE Umicom::base)
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.environment.consumer COMMAND umicom-sdk-environment-example)
endif()
