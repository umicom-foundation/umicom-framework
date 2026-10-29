#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/learning_pack/check_search_evidence.cmake
#
# PURPOSE:
#   Prevent output text from masking an unsuccessful SDK search process.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED FRAMEWORK_ROOT OR NOT DEFINED CASE)
    message(FATAL_ERROR "Supply FRAMEWORK_ROOT and CASE")
endif()
set(UMICOM_LEARNING_SEARCH_VALIDATE_ONLY ON)
include("${FRAMEWORK_ROOT}/examples/learning_pack_inspector/check_search.cmake")
set(_result 0)
set(_output "Matches: 1; returned: 1; truncated: no\nframework.learning-packs::reader-history | History\n")
set(_identity "framework.learning-packs::reader-history")
set(_expected_success OFF)
if(CASE STREQUAL "valid")
    set(_expected_success ON)
elseif(CASE STREQUAL "crlf")
    string(REPLACE "\n" "\r\n" _output "${_output}")
    set(_expected_success ON)
elseif(CASE STREQUAL "failed-process")
    set(_result 1)
elseif(CASE STREQUAL "timeout")
    set(_result "Process terminated due to timeout")
elseif(CASE STREQUAL "wrong-count")
    string(REPLACE "Matches: 1" "Matches: 2" _output "${_output}")
elseif(CASE STREQUAL "wrong-id")
    set(_identity "framework.learning-packs::absent")
elseif(CASE STREQUAL "empty")
    set(_output "")
elseif(CASE STREQUAL "truncated")
    string(REPLACE "truncated: no" "truncated: yes" _output "${_output}")
elseif(CASE STREQUAL "prefixed")
    string(PREPEND _output "Unexpected output\n")
else()
    message(FATAL_ERROR "Unknown search evidence case: ${CASE}")
endif()
umicom_validate_learning_search("${_result}" "${_output}" 1 "${_identity}" _reason)
if(_expected_success AND NOT _reason STREQUAL "")
    message(FATAL_ERROR "Valid evidence was refused: ${_reason}")
elseif(NOT _expected_success AND _reason STREQUAL "")
    message(FATAL_ERROR "Invalid evidence was accepted")
endif()
