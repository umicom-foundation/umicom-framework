#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_qualification/check_plan_cli.cmake
#
# PURPOSE:
#   Require successful fixture execution and exact public selection summaries.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED TOOL OR NOT DEFINED CASE)
    message(FATAL_ERROR "TOOL and CASE are required")
endif()
if(CASE STREQUAL "source-help" OR CASE STREQUAL "plan-help")
    if(CASE STREQUAL "source-help")
        set(_command source-observation)
    else()
        set(_command qualification-plan)
    endif()
    execute_process(COMMAND "${TOOL}" "${_command}" --help
        RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error
        ENCODING UTF-8 TIMEOUT 20)
    if(NOT _result STREQUAL "0" OR NOT _output MATCHES "Usage: umicom ${_command}")
        message(FATAL_ERROR "Command help failed: ${_result}\n${_output}\n${_error}")
    endif()
    return()
endif()
execute_process(COMMAND "${TOOL}" "${CASE}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _output ERROR_VARIABLE _error
    ENCODING UTF-8 TIMEOUT 40)
if(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Planning fixture failed: ${_result}\n${_output}\n${_error}")
endif()
if(CASE STREQUAL "passed" OR CASE STREQUAL "summary")
    set(_expected "Selected: 0 targets; 0 tests.")
elseif(CASE STREQUAL "failed")
    set(_expected "Selected: 1 targets; 1 tests.")
elseif(CASE STREQUAL "changed" OR CASE STREQUAL "no-history" OR CASE STREQUAL "unknown-tools" OR CASE STREQUAL "outside-root")
    set(_expected "Selected: 1 targets; 2 tests.")
endif()
if(DEFINED _expected)
    string(FIND "${_output}" "${_expected}" _selection)
    string(FIND "${_output}" "PREVIEW ONLY. No build or test executed" _preview)
    if(_selection LESS 0 OR _preview LESS 0)
        message(FATAL_ERROR "Unexpected plan output:\n${_output}")
    endif()
endif()
if(CASE STREQUAL "failed")
    string(FIND "${_output}" "test\ttest.two\t" _required)
    string(FIND "${_output}" "test\ttest.one\t" _passed)
    if(_required LESS 0 OR NOT _passed EQUAL -1)
        message(FATAL_ERROR "Failed test selection differs:\n${_output}")
    endif()
endif()
if(CASE STREQUAL "summary" AND _output MATCHES "\n(target|test)\t")
    message(FATAL_ERROR "Summary unexpectedly emitted selection rows")
endif()

# An unresolved source still widens coverage; fixing a fixture must not remove
# the conservative planner protection that exposed its incorrect root.
if(CASE STREQUAL "outside-root")
    foreach(_required IN ITEMS "test\ttest.one\t" "test\ttest.two\t" "graph reasons: 0x20.")
        string(FIND "${_output}" "${_required}" _position)
        if(_position LESS 0)
            message(FATAL_ERROR "Outside-root fallback missing: ${_required}\n${_output}")
        endif()
    endforeach()
endif()
