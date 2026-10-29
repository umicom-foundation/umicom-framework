#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning_pack_inspector/check_transaction.cmake
#
# PURPOSE:
#   Require both process status and retained-reader evidence from the SDK example.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED PROGRAM OR NOT EXISTS "${PROGRAM}" OR
   NOT DEFINED PACK_ROOT OR NOT IS_DIRECTORY "${PACK_ROOT}")
    message(FATAL_ERROR "Supply the built program and generated pack directory")
endif()
set(_target inspector)
set(_decision accept)
set(_code 0)
set(_output "Decision: accepted\nResource: inspector\nHistory entries: 2\n")
if(CASE STREQUAL "reject")
    set(_decision reject)
    set(_output "Decision: rejected\nResource: guide\nHistory entries: 1\n")
elseif(CASE STREQUAL "missing")
    set(_target missing-resource)
    set(_code 1)
    set(_output "Decision: refused\nResource: guide\nHistory entries: 1\n")
elseif(CASE STREQUAL "invalid-decision")
    set(_decision automatic)
    set(_code 2)
    set(_output "")
elseif(NOT CASE STREQUAL "accept")
    message(FATAL_ERROR "Unknown transaction example case")
endif()
execute_process(COMMAND "${PROGRAM}" "${PACK_ROOT}" learning-pack.json guide "${_target}" "${_decision}"
    RESULT_VARIABLE _actual OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr TIMEOUT 20)
string(REPLACE "\r\n" "\n" _stdout "${_stdout}")
if(NOT "${_actual}" STREQUAL "${_code}" OR NOT "${_stdout}" STREQUAL "${_output}")
    message(FATAL_ERROR "Candidate decision evidence mismatch (exit ${_actual})\n${_stdout}\n${_stderr}")
endif()
if(CASE STREQUAL "missing" AND NOT _stderr MATCHES "Candidate refused:")
    message(FATAL_ERROR "Missing failure explanation")
endif()
if(CASE STREQUAL "invalid-decision" AND NOT _stderr MATCHES "Usage:")
    message(FATAL_ERROR "Missing usage explanation")
endif()
if(_code EQUAL 0 AND NOT _stderr STREQUAL "")
    message(FATAL_ERROR "Unexpected diagnostic on successful decision: ${_stderr}")
endif()
