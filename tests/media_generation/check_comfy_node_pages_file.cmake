# Umicom Framework
# File: tests/media_generation/check_comfy_node_pages_file.cmake
#
# PURPOSE:
#   Drive native file admission with isolated fixtures and verify source bytes remain unchanged.
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
include("${CMAKE_CURRENT_LIST_DIR}/comfy_node_pages_fixture.cmake")
umicom_comfy_node_pages_fixture("${TEST_ROOT}" "${TEST_CASE}")
file(SHA256 "${NODE_DOCUMENT}" document_before)
file(SHA256 "${NODE_DATA_FILE}" data_before)
execute_process(COMMAND "${TEST_EXE}" "${TEST_CASE}" "${NODE_DOCUMENT}" "${NODE_INPUT}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 35)
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Node file case ${TEST_CASE}: ${result}\n${output}\n${error}")
endif()
file(SHA256 "${NODE_DOCUMENT}" document_after)
file(SHA256 "${NODE_DATA_FILE}" data_after)
if(NOT document_before STREQUAL document_after OR NOT data_before STREQUAL data_after)
    message(FATAL_ERROR "Node selection read modified its source documents")
endif()
