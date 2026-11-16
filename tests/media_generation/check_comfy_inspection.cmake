# Umicom Framework
# File: tests/media_generation/check_comfy_inspection.cmake
#
# PURPOSE:
#   Run one isolated native file or JSON contract case and preserve diagnostic output.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(NOT DEFINED TEST_EXE OR NOT DEFINED TEST_ROOT OR NOT DEFINED TEST_CASE)
    message(FATAL_ERROR "TEST_EXE, TEST_ROOT and TEST_CASE are required")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/comfy_inspection_fixture.cmake")
umicom_comfy_inspection_fixture("${TEST_ROOT}" "${TEST_CASE}")
execute_process(COMMAND "${TEST_EXE}" "${TEST_CASE}"
    "${INSPECTION_CONNECTION}" "${INSPECTION_WORKFLOW}" "${INSPECTION_NODES}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 35)
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Inspection contract ${TEST_CASE}: ${result}\n${output}\n${error}")
endif()
