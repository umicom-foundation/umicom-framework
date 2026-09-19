#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/gui_recording/check_example_failure.cmake
# PURPOSE: Distinguish an expected example finding from a crash or setup error.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT EXISTS "${PROGRAM}")
    message(FATAL_ERROR "Example executable is missing: ${PROGRAM}")
endif()
execute_process(COMMAND "${PROGRAM}" "${ARGUMENT}"
    RESULT_VARIABLE _exit OUTPUT_VARIABLE _output ERROR_VARIABLE _error TIMEOUT 10)
if(NOT "${_exit}" STREQUAL "1")
    message(FATAL_ERROR "Expected an ordinary finding (exit 1), received '${_exit}'.\n${_error}\n${_output}")
endif()
# Read the actual report. A setup error or arbitrary nonzero exit is not success.
if(ARGUMENT STREQUAL "--duplicate")
    string(JSON _schema GET "${_output}" schema)
    string(JSON _count GET "${_output}" summary ambiguous_targets)
    if(NOT _schema STREQUAL "umicom.ui.controls" OR NOT _count EQUAL 2)
        message(FATAL_ERROR "The duplicate example did not report both ambiguous controls.")
    endif()
elseif(ARGUMENT STREQUAL "--fail-save")
    string(JSON _count GET "${_output}" count)
    string(JSON _target GET "${_output}" entries 1 target_id)
    string(JSON _failed GET "${_output}" entries 1 status)
    if(NOT _count EQUAL 2 OR NOT _target STREQUAL "notes.saved-file" OR _failed EQUAL 0)
        message(FATAL_ERROR "The no-op example did not fail its saved-file assertion.")
    endif()
else()
    message(FATAL_ERROR "Unknown negative example argument: ${ARGUMENT}")
endif()
message(STATUS "Expected finding verified in the example report.")
