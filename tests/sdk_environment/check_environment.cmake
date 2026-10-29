#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/sdk_environment/check_environment.cmake
#
# PURPOSE:
#   Check SDK environment refusal rules without pretending to test binary ABI.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED FRAMEWORK OR NOT DEFINED CASE)
    message(FATAL_ERROR "Supply FRAMEWORK and CASE")
endif()
set(_cases matching version-reporting abi system pointer compiler missing-producer missing-consumer invalid-pointer argument)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown SDK environment scenario")
endif()
if(NOT CASE MATCHES "^(matching|version-reporting)$" AND NOT PROBE)
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK=${FRAMEWORK}"
        "-DCASE=${CASE}" -DPROBE=ON -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
    if(_result STREQUAL "0" OR NOT _error MATCHES "SDK")
        message(FATAL_ERROR "Expected a declared-environment refusal: ${_output}\n${_error}")
    endif()
    return()
endif()
# These are explicit metadata fixtures. They do not inspect the host compiler,
# link an archive, launch an application or establish a supported target matrix.
set(UMICOM_FRAMEWORK_ABI_VERSION 2)
set(UMICOM_FRAMEWORK_TARGET_SYSTEM FixtureOS)
set(UMICOM_FRAMEWORK_TARGET_POINTER_BYTES 8)
set(UMICOM_FRAMEWORK_PRODUCER_C_COMPILER FixtureC)
set(UMICOM_FRAMEWORK_PRODUCER_C_COMPILER_VERSION 1.2)
set(CMAKE_SYSTEM_NAME FixtureOS)
set(CMAKE_SIZEOF_VOID_P 8)
set(CMAKE_C_COMPILER_ID FixtureC)
set(CMAKE_C_COMPILER_VERSION 1.2)
set(_required 2)
if(CASE STREQUAL "version-reporting")
    set(CMAKE_C_COMPILER_VERSION 1.3)
elseif(CASE STREQUAL "abi")
    set(_required 3)
elseif(CASE STREQUAL "system")
    set(CMAKE_SYSTEM_NAME OtherOS)
elseif(CASE STREQUAL "pointer")
    set(CMAKE_SIZEOF_VOID_P 4)
elseif(CASE STREQUAL "compiler")
    set(CMAKE_C_COMPILER_ID OtherC)
elseif(CASE STREQUAL "missing-producer")
    unset(UMICOM_FRAMEWORK_TARGET_SYSTEM)
elseif(CASE STREQUAL "missing-consumer")
    unset(CMAKE_C_COMPILER_ID)
elseif(CASE STREQUAL "invalid-pointer")
    set(UMICOM_FRAMEWORK_TARGET_POINTER_BYTES 0)
elseif(CASE STREQUAL "argument")
    set(_required 0)
endif()
include("${FRAMEWORK}/cmake/UmicomRequireSdkEnvironment.cmake")
umicom_require_sdk_environment("${_required}")
if(NOT CASE MATCHES "^(matching|version-reporting)$")
    message(FATAL_ERROR "Incompatible fixture was unexpectedly accepted")
endif()
