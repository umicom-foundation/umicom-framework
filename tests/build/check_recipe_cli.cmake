#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/build/check_recipe_cli.cmake
#
# PURPOSE:
#   Check the real native recipe route without executing any recipe stage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

if(NOT DEFINED TOOL OR NOT DEFINED FILE OR NOT DEFINED CASE)
    message(FATAL_ERROR "TOOL, FILE and CASE are required")
endif()
execute_process(COMMAND "${TOOL}" recipe inspect --file "${FILE}" RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 20)
if(CASE STREQUAL "minimal")
    string(FIND "${output}" "Recipe: source-demo" identity)
    string(FIND "${output}" "Inputs: 1 | Stages: 1" counts)
    string(FIND "${output}" "no stages executed" boundary)
    if(NOT result EQUAL 0 OR identity EQUAL -1 OR counts EQUAL -1 OR boundary EQUAL -1)
        message(FATAL_ERROR "Inspection mismatch: ${result} ${output} ${error}")
    endif()
elseif(CASE STREQUAL "cycle" OR CASE STREQUAL "unknown-stage")
    string(FIND "${error}" "Recipe refused:" refusal)
    if(NOT result EQUAL 1 OR refusal EQUAL -1 OR NOT output STREQUAL "")
        message(FATAL_ERROR "Expected refusal: ${result} ${output} ${error}")
    endif()
else()
    message(FATAL_ERROR "Unknown entry scenario")
endif()
