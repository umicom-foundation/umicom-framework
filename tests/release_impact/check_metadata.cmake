#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_impact/check_metadata.cmake
# Purpose: Retain nested configuration diagnostics for impact metadata checks.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED ROOT OR NOT DEFINED DEST OR NOT DEFINED CASE)
    message(FATAL_ERROR "ROOT, DEST and CASE are required")
endif()
file(MAKE_DIRECTORY "${DEST}")
execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/release_impact/fixture" -B "${DEST}" -G Ninja
    "-DROOT=${ROOT}" "-DCASE=${CASE}"
    RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/configure.txt" ERROR_FILE "${DEST}/configure-errors.txt")
file(READ "${DEST}/configure-errors.txt" _errors)
if(CASE STREQUAL "bad-owner")
    if(_result STREQUAL "0" OR NOT _errors MATCHES "Unknown impact test owner")
        message(FATAL_ERROR "Unknown test prerequisite was not refused; see ${DEST}")
    endif()
elseif(CASE STREQUAL "bad-test")
    if(_result STREQUAL "0" OR NOT _errors MATCHES "Declare impact ownership after add_test")
        message(FATAL_ERROR "Unknown test registration was not refused; see ${DEST}")
    endif()
elseif(NOT _result STREQUAL "0")
    message(FATAL_ERROR "Impact composition failed; see ${DEST}/configure-errors.txt")
endif()
