#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning_pack_inspector/check_help.cmake
#
# PURPOSE:
#   Require a verified guide outcome as well as the expected help process status.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(_required PROGRAM PACK_ROOT CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing ${_required}")
    endif()
endforeach()
set(_kind "${CASE}")
set(_expected 0)
if(CASE STREQUAL "command")
    set(_key help.documentation)
elseif(CASE STREQUAL "diagnostic")
    set(_key learning.content-changed)
elseif(CASE STREQUAL "topic")
    set(_key learning-packs)
elseif(CASE STREQUAL "missing")
    set(_kind command)
    set(_key absent.command)
    set(_expected 4)
else()
    message(FATAL_ERROR "Unknown help case: ${CASE}")
endif()
execute_process(COMMAND "${PROGRAM}" "${PACK_ROOT}" learning-pack.json "${_kind}" "${_key}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 30)
if(NOT "${_result}" STREQUAL "${_expected}")
    message(FATAL_ERROR "Help process status ${_result}; expected ${_expected}\n${_output}\n${_error}")
endif()
if(_expected EQUAL 0)
    file(SIZE "${PACK_ROOT}/guide.html" _guide_bytes)
    string(REPLACE "\r\n" "\n" _output "${_output}")
    set(_expected_output "Help: ${_key} -> framework.learning-packs::guide\nVerified bytes: ${_guide_bytes}\n")
    if(NOT "${_output}" STREQUAL "${_expected_output}")
        message(FATAL_ERROR "Help output does not identify the complete verified guide: ${_output}")
    endif()
elseif(NOT "${_output}" STREQUAL "")
    message(FATAL_ERROR "Missing help unexpectedly published a successful result: ${_output}")
endif()
