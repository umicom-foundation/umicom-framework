#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/learning_pack_inspector/check_search.cmake
#
# PURPOSE:
#   Require both a successful process and the expected catalogue output.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
function(umicom_validate_learning_search result output count identity out_reason)
    set(_reason "")
    if(NOT "${result}" STREQUAL "0")
        set(_reason "Search process failed or did not finish: ${result}")
    else()
        string(REPLACE "\r\n" "\n" _output "${output}")
        set(_summary "Matches: ${count}; returned: ${count}; truncated: no\n")
        string(FIND "${_output}" "${_summary}" _summary_position)
        if(NOT _summary_position EQUAL 0)
            set(_reason "Search summary differs from the expected complete result")
        elseif(NOT "${identity}" STREQUAL "")
            string(FIND "${_output}" "\n${identity} |" _identity_position)
            if(_identity_position EQUAL -1)
                set(_reason "Search output is missing the expected resource identity")
            endif()
        endif()
    endif()
    set(${out_reason} "${_reason}" PARENT_SCOPE)
endfunction()
# Focused contract tests import the same validator without launching a program.
if(UMICOM_LEARNING_SEARCH_VALIDATE_ONLY)
    return()
endif()
foreach(_required PROGRAM PACK_ROOT CASE)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing ${_required}")
    endif()
endforeach()
set(_arguments "${PACK_ROOT}" learning-pack.json)
set(_identity "")
if(CASE STREQUAL "browse")
    file(READ "${PACK_ROOT}/learning-pack.json" _manifest)
    string(JSON _count LENGTH "${_manifest}" resources)
elseif(CASE STREQUAL "history")
    list(APPEND _arguments history framework en)
    set(_count 1)
    set(_identity "framework.learning-packs::reader-history")
elseif(CASE STREQUAL "missing")
    list(APPEND _arguments no-such-topic)
    set(_count 0)
else()
    message(FATAL_ERROR "Unknown search check: ${CASE}")
endif()
execute_process(COMMAND "${PROGRAM}" ${_arguments}
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 30)
umicom_validate_learning_search("${_result}" "${_output}" "${_count}" "${_identity}" _reason)
if(NOT _reason STREQUAL "")
    message(FATAL_ERROR "${_reason}\nstdout:\n${_output}\nstderr:\n${_error}")
endif()
