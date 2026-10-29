#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_reuse/check_protocol.cmake
#
# PURPOSE:
#   Reject stale helper interfaces and verify the native command handshake.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.29)
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomTestReceiptProtocol.cmake")
if(NOT DEFINED CASE)
    message(FATAL_ERROR "Missing protocol scenario")
endif()
set(_accepted FALSE)
set(_result 0)
set(_output "UMICOM-TEST-RECEIPTS 2\n")
if(CASE STREQUAL "native")
    if(NOT DEFINED PROGRAM OR NOT DEFINED WORK)
        message(FATAL_ERROR "Missing native protocol fixture inputs")
    endif()
    string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
    file(MAKE_DIRECTORY "${WORK}")
    umicom_check_test_receipt_protocol("${PROGRAM}" "${WORK}/${_nonce}" _reason)
    if(NOT _reason STREQUAL "")
        message(FATAL_ERROR "${_reason}")
    endif()
    return()
elseif(CASE STREQUAL "lf")
    set(_accepted TRUE)
elseif(CASE STREQUAL "crlf")
    set(_output "UMICOM-TEST-RECEIPTS 2\r\n")
    set(_accepted TRUE)
elseif(CASE STREQUAL "stale-usage")
    set(_result 1)
    set(_output "Usage: umicom-test-receipts prepare MANIFEST STATE EXCLUSIONS | record STATE JUNIT\n")
elseif(CASE STREQUAL "old-version")
    set(_output "UMICOM-TEST-RECEIPTS 1\n")
elseif(CASE STREQUAL "empty")
    set(_output "")
elseif(CASE STREQUAL "extra-output")
    string(APPEND _output "unrelated message\n")
elseif(CASE STREQUAL "failed-query")
    # A matching banner cannot excuse a failed process.
    set(_result 1)
elseif(CASE STREQUAL "timeout")
    set(_result "Process terminated due to timeout")
elseif(CASE STREQUAL "bare-cr")
    set(_output "UMICOM-TEST-RECEIPTS 2\r")
else()
    message(FATAL_ERROR "Unknown protocol scenario: ${CASE}")
endif()
umicom_validate_test_receipt_protocol("${_result}" "${_output}" _reason)
if(_accepted AND NOT _reason STREQUAL "")
    message(FATAL_ERROR "Compatible protocol was refused: ${_reason}")
elseif(NOT _accepted AND _reason STREQUAL "")
    message(FATAL_ERROR "Incompatible helper was accepted")
endif()
