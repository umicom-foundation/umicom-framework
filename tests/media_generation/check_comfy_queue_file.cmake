# Umicom Framework
# File: tests/media_generation/check_comfy_queue_file.cmake
#
# PURPOSE:
#   Run isolated native queue-file cases and verify selected fixture files remain unchanged.
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
include("${CMAKE_CURRENT_LIST_DIR}/comfy_queue_fixture.cmake")
umicom_comfy_queue_fixture("${TEST_ROOT}" "${TEST_CASE}")
file(SHA256 "${QUEUE_DOCUMENT_FILE}" document_before)
file(SHA256 "${QUEUE_DATA_FILE}" queue_before)
execute_process(COMMAND "${TEST_EXE}" "${TEST_CASE}" "${QUEUE_DOCUMENT}" "${QUEUE_INPUT}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 30)
file(SHA256 "${QUEUE_DOCUMENT_FILE}" document_after)
file(SHA256 "${QUEUE_DATA_FILE}" queue_after)
if(NOT document_before STREQUAL document_after OR NOT queue_before STREQUAL queue_after)
    message(FATAL_ERROR "Queue inspector changed its fixture files")
endif()
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Queue file ${TEST_CASE}: ${result}\n${output}\n${error}")
endif()
