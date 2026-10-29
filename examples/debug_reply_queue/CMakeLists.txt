#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/debug_reply_queue/CMakeLists.txt
#
# PURPOSE:
#   Build a debugger queue example against one explicit installed Framework SDK.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomDebugReplyQueue LANGUAGES C)
if(NOT DEFINED UMICOM_EXPECTED_SDK_PREFIX OR
   NOT IS_ABSOLUTE "${UMICOM_EXPECTED_SDK_PREFIX}" OR
   NOT IS_DIRECTORY "${UMICOM_EXPECTED_SDK_PREFIX}")
    message(FATAL_ERROR "Supply UMICOM_EXPECTED_SDK_PREFIX as the installed SDK's absolute directory")
endif()
find_package(UmicomFramework CONFIG REQUIRED COMPONENTS developer
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
umicom_require_installed_sdk_layout("${UMICOM_EXPECTED_SDK_PREFIX}" _headers Umicom::developer)
add_executable(umicom-debug-reply-queue main.c)
target_link_libraries(umicom-debug-reply-queue PRIVATE Umicom::developer)
set_target_properties(umicom-debug-reply-queue PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(MSVC)
    target_compile_options(umicom-debug-reply-queue PRIVATE /W4)
else()
    target_compile_options(umicom-debug-reply-queue PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()


include(CTest)
if(BUILD_TESTING)
    foreach(_case IN ITEMS ordered interleaved remote-error)
        add_test(NAME sdk.debug_reply_queue.${_case} COMMAND umicom-debug-reply-queue ${_case})
        set_tests_properties(sdk.debug_reply_queue.${_case} PROPERTIES TIMEOUT 30)
    endforeach()
endif()
