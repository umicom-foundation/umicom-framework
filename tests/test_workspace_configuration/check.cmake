#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_workspace_configuration/check.cmake
# PURPOSE: Verify explicit test workspace configuration with isolated fixtures.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
# This test runs a miniature configure, never the product build or a user project.
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${FIXTURE_SOURCE}" -B "${FIXTURE_BUILD}" "-DCASE=${CASE}"
    RESULT_VARIABLE _result OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
if(CASE MATCHES "^invalid-")
    if(_result EQUAL 0 OR NOT "${_stdout}${_stderr}" MATCHES "UMICOM_TEST_WORK_ROOT must")
        message(FATAL_ERROR "Invalid fixture root was not rejected by its own contract: ${_stdout}${_stderr}")
    endif()
elseif(NOT _result EQUAL 0)
    message(FATAL_ERROR "Fixture configuration failed: ${_stdout}${_stderr}")
endif()
