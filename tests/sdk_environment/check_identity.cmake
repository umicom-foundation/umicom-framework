#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/sdk_environment/check_identity.cmake
#
# PURPOSE:
#   Keep public version definitions, package identity and receipt identity aligned.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(_name FRAMEWORK WORK CASE)
    if(NOT DEFINED ${_name} OR "${${_name}}" STREQUAL "")
        message(FATAL_ERROR "Missing identity fixture input: ${_name}")
    endif()
endforeach()
set(_valid current maximum zero-version)
set(_invalid duplicate missing negative leading-zero abi-zero abi-overflow version-overflow string-mismatch)
if(NOT CASE IN_LIST _valid AND NOT CASE IN_LIST _invalid)
    message(FATAL_ERROR "Unknown identity fixture case")
endif()
if(CASE IN_LIST _invalid AND NOT PROBE)
    execute_process(COMMAND "${CMAKE_COMMAND}" "-DFRAMEWORK=${FRAMEWORK}" "-DWORK=${WORK}"
        "-DCASE=${CASE}" -DPROBE=ON -P "${CMAKE_CURRENT_LIST_FILE}"
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
    if(_result STREQUAL "0" OR NOT _error MATCHES "Framework identity")
        message(FATAL_ERROR "Expected an identity refusal: ${_output}\n${_error}")
    endif()
    return()
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
set(_root "${WORK}/${_nonce}")
file(MAKE_DIRECTORY "${_root}")
set(_major 0)
set(_minor 9)
set(_patch 0)
set(_abi 2)
if(CASE STREQUAL "maximum")
    set(_major 65535)
    set(_minor 65535)
    set(_patch 65535)
    set(_abi 4294967295)
elseif(CASE STREQUAL "zero-version")
    set(_minor 0)
elseif(CASE STREQUAL "negative")
    set(_minor -1)
elseif(CASE STREQUAL "leading-zero")
    set(_minor 09)
elseif(CASE STREQUAL "abi-zero")
    set(_abi 0)
elseif(CASE STREQUAL "abi-overflow")
    set(_abi 4294967296)
elseif(CASE STREQUAL "version-overflow")
    set(_patch 65536)
endif()
set(_version "${_major}.${_minor}.${_patch}")
if(CASE STREQUAL "string-mismatch")
    set(_version "1.0.0")
endif()
set(_text "#define UMICOM_FRAMEWORK_VERSION_MAJOR ${_major}\n#define UMICOM_FRAMEWORK_VERSION_MINOR ${_minor}\n#define UMICOM_FRAMEWORK_VERSION_PATCH ${_patch}\n#define UMICOM_FRAMEWORK_VERSION_STRING \"${_version}\"\n#define UMICOM_FRAMEWORK_ABI_VERSION ${_abi}U\n")
if(CASE STREQUAL "duplicate")
    string(APPEND _text "#define UMICOM_FRAMEWORK_ABI_VERSION 3U\n")
elseif(CASE STREQUAL "missing")
    string(REPLACE "#define UMICOM_FRAMEWORK_ABI_VERSION ${_abi}U\n" "" _text "${_text}")
endif()
file(WRITE "${_root}/version.h" "${_text}")
include("${FRAMEWORK}/cmake/UmicomFrameworkIdentity.cmake")
umicom_read_framework_identity("${_root}/version.h" fixture)
if(NOT fixture_VERSION STREQUAL _version OR NOT fixture_ABI STREQUAL _abi)
    message(FATAL_ERROR "Unexpected extracted values")
endif()
if(CASE IN_LIST _invalid)
    message(FATAL_ERROR "Malformed fixture was unexpectedly accepted")
endif()
