#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/background_services/CMakeLists.txt
#
# PURPOSE:
#   Consume background services exclusively from one installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomBackgroundServicesExample LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply the installed SDK's absolute directory as UMICOM_EXPECTED_SDK_PREFIX")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS platform
    PATHS "${UMICOM_EXPECTED_SDK_PREFIX}" NO_DEFAULT_PATH NO_CMAKE_FIND_ROOT_PATH)
if(NOT DEFINED UmicomFramework_CONFIG OR NOT EXISTS "${UmicomFramework_CONFIG}")
    message(FATAL_ERROR "The selected Framework configuration is not an existing file")
endif()
# Cached package choices must not defeat the consumer's explicit origin.
file(REAL_PATH "${UMICOM_EXPECTED_SDK_PREFIX}" _sdk_prefix)
file(REAL_PATH "${UmicomFramework_CONFIG}" _sdk_configuration)
cmake_path(IS_PREFIX _sdk_prefix "${_sdk_configuration}" NORMALIZE _sdk_inside)
if(NOT _sdk_inside)
    message(FATAL_ERROR "The selected Framework package is outside the requested prefix")
endif()
if(NOT COMMAND umicom_require_sdk_environment)
    message(FATAL_ERROR "Reinstall Framework with the environment metadata helpers")
endif()
umicom_require_sdk_environment(2)
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::platform)
add_executable(umicom-background-services-example main.c)
target_link_libraries(umicom-background-services-example PRIVATE Umicom::platform)
set_target_properties(umicom-background-services-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-background-services-example PRIVATE /W4)
else()
    target_compile_options(umicom-background-services-example PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME sdk.background_services.consumer
        COMMAND umicom-background-services-example --verify
            "$<TARGET_FILE:umicom-background-services-example>")
    set_tests_properties(sdk.background_services.consumer PROPERTIES TIMEOUT 30)
endif()
