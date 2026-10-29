#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_reuse/check_command_owners.cmake
#
# PURPOSE:
#   Verify registry ownership through the native file adapter without running tests.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.29)
foreach(_name PROGRAM WORK CASE)
    if(NOT DEFINED ${_name} OR "${${_name}}" STREQUAL "")
        message(FATAL_ERROR "Missing fixture input: ${_name}")
    endif()
endforeach()
set(_cases matching crlf empty wrong-program wrong-name duplicate wrapper
    missing-command relative malformed-row duplicate-catalogue)
if(NOT CASE IN_LIST _cases)
    message(FATAL_ERROR "Unknown command ownership scenario")
endif()
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
set(_root "${WORK}/${_nonce}/owner with spaces")
file(MAKE_DIRECTORY "${_root}")
# Negative cases must reach the intended verifier, not an older helper's usage
# error. Retain the handshake separately from each fixture's immutable inputs.
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomTestReceiptProtocol.cmake")
umicom_check_test_receipt_protocol("${PROGRAM}" "${_root}/protocol" _protocol_error)
if(NOT _protocol_error STREQUAL "")
    message(FATAL_ERROR "${_protocol_error}")
endif()

set(_owner "${_root}/wanted.exe")
set(_actual "${_owner}")
set(_registry "fixture.alpha\t${_owner}\n")
set(_expected 0)
set(_newline LF)
if(CASE STREQUAL "crlf")
    set(_newline CRLF)
elseif(CASE STREQUAL "empty")
    set(_registry "")
elseif(CASE STREQUAL "wrong-program")
    set(_actual "${_root}/other.exe")
    set(_expected 1)
elseif(CASE STREQUAL "wrong-name")
    set(_registry "fixture.missing\t${_owner}\n")
    set(_expected 1)
elseif(CASE STREQUAL "duplicate")
    string(APPEND _registry "${_registry}")
    set(_expected 1)
elseif(CASE STREQUAL "wrapper")
    set(_actual "${CMAKE_COMMAND}")
    set(_expected 1)
elseif(CASE STREQUAL "relative")
    set(_owner "wanted.exe")
    set(_actual "${_owner}")
    set(_registry "fixture.alpha\t${_owner}\n")
    set(_expected 1)
elseif(CASE STREQUAL "malformed-row")
    set(_registry "fixture.alpha\t${_owner}\textra\n")
    set(_expected 1)
endif()
# Only the helper runs. These strings name no real binary or broker operation.
# JSON escaping retains spaces, separators and quotes in a fixture directory.
string(REPLACE "\\" "\\\\" _escaped "${_actual}")
string(REPLACE "\"" "\\\"" _escaped "${_escaped}")
set(_test "{\"name\":\"fixture.alpha\",\"command\":[\"${_escaped}\",\"--fixture\"]}")
if(CASE STREQUAL "missing-command")
    set(_test "{\"name\":\"fixture.alpha\"}")
    set(_expected 1)
elseif(CASE STREQUAL "duplicate-catalogue")
    string(APPEND _test ",${_test}")
    set(_expected 1)
endif()
set(_json "{\"kind\":\"ctestInfo\",\"version\":{\"major\":1,\"minor\":0},\"tests\":[${_test}]}")
file(CONFIGURE OUTPUT "${_root}/registry.tsv" CONTENT "${_registry}" @ONLY NEWLINE_STYLE ${_newline})
file(CONFIGURE OUTPUT "${_root}/discovery.json" CONTENT "${_json}" @ONLY NEWLINE_STYLE LF)
file(SHA256 "${_root}/registry.tsv" _before_registry)
file(SHA256 "${_root}/discovery.json" _before_json)
execute_process(COMMAND "${PROGRAM}" verify-inputs "${_root}/registry.tsv" "${_root}/discovery.json"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 20)
if(NOT _result STREQUAL "${_expected}")
    message(FATAL_ERROR "Ownership result ${_result}, expected ${_expected}: ${_output}\n${_error}")
endif()
file(SHA256 "${_root}/registry.tsv" _after_registry)
file(SHA256 "${_root}/discovery.json" _after_json)
if(NOT _before_registry STREQUAL _after_registry OR NOT _before_json STREQUAL _after_json)
    message(FATAL_ERROR "Ownership verification changed its input files")
endif()
