#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/service_composition/CMakeLists.txt
#
# PURPOSE:
#   Consume module composition exclusively from one installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomServiceCompositionExample LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply the installed SDK's absolute directory as UMICOM_EXPECTED_SDK_PREFIX")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS runtime
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
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::runtime)
add_executable(umicom-service-composition-example main.c services.c composition.h)
target_link_libraries(umicom-service-composition-example PRIVATE Umicom::runtime)
set_target_properties(umicom-service-composition-example PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-service-composition-example PRIVATE /W4)
else()
    target_compile_options(umicom-service-composition-example PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    foreach(_case IN ITEMS normal corrected-registration startup-failure stop-retry restart)
        add_test(NAME sdk.service_composition.${_case}
            COMMAND umicom-service-composition-example "${_case}")
        set_tests_properties(sdk.service_composition.${_case} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
