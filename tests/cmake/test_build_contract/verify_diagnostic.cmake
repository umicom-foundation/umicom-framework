#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_build_contract/verify_diagnostic.cmake
# PURPOSE: Verify a deliberate failure reports its expression and returns 1.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(NOT DEFINED TEST_EXECUTABLE OR NOT EXISTS "${TEST_EXECUTABLE}")
    message(FATAL_ERROR "Build the diagnostic test executable before running CTest.")
endif()
execute_process(COMMAND "${TEST_EXECUTABLE}" --fail
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error
    TIMEOUT 15)
if(NOT "${result}" STREQUAL "1")
    message(FATAL_ERROR "Expected normal exit 1, got '${result}'.\n${output}\n${error}")
endif()
if(NOT error MATCHES "test_check_header[.]c:[0-9]+: test check failed: 0")
    message(FATAL_ERROR "The source/expression diagnostic was not reported:\n${error}")
endif()
if(error MATCHES "UNREACHABLE_AFTER_FAILED_CHECK")
    message(FATAL_ERROR "A failing check incorrectly continued execution.")
endif()
message(STATUS "Diagnostic failure returned 1 and identified the failed source expression.")
