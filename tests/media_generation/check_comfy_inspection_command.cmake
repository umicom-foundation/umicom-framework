# Umicom Framework
# File: tests/media_generation/check_comfy_inspection_command.cmake
#
# PURPOSE:
#   Verify shared console exit codes, JSON provenance and optional CLI versus Media equivalence.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT

if(NOT DEFINED HOST_EXE OR NOT DEFINED TEST_ROOT OR NOT DEFINED TEST_CASE)
    message(FATAL_ERROR "HOST_EXE, TEST_ROOT and TEST_CASE are required")
endif()
include("${CMAKE_CURRENT_LIST_DIR}/comfy_inspection_fixture.cmake")
umicom_comfy_inspection_fixture("${TEST_ROOT}" "${TEST_CASE}")
set(args comfy inspect "${INSPECTION_CONNECTION}" "${INSPECTION_WORKFLOW}" "${INSPECTION_NODES}")
set(expected 0)
if(TEST_CASE STREQUAL "missing")
    set(expected 3)
elseif(TEST_CASE STREQUAL "malformed-document")
    set(expected 1)
elseif(TEST_CASE STREQUAL "usage")
    set(args comfy inspect)
    set(expected 2)
elseif(TEST_CASE STREQUAL "help")
    set(args comfy --help)
endif()
execute_process(COMMAND "${HOST_EXE}" ${args}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 35)
if(NOT result STREQUAL "${expected}")
    message(FATAL_ERROR "Console returned ${result}; expected ${expected}: ${error}")
endif()
if(TEST_CASE STREQUAL "complete" OR TEST_CASE STREQUAL "missing" OR TEST_CASE STREQUAL "unicode-path")
    string(JSON origin GET "${output}" origin)
    string(JSON scope GET "${output}" report scope)
    string(JSON observed GET "${output}" report all_node_requirements_observed)
    string(JSON revision GET "${output}" report connection_revision)
    if(NOT origin STREQUAL "offline-files" OR NOT scope STREQUAL "class-and-required-field-presence" OR NOT revision STREQUAL "7")
        message(FATAL_ERROR "Console mislabels its observation")
    endif()
    if(TEST_CASE STREQUAL "missing")
        if(observed)
            message(FATAL_ERROR "Missing field was reported as present")
        endif()
    elseif(NOT observed)
        message(FATAL_ERROR "Complete fixture was not observed")
    endif()
    if(output MATCHES "PRIVATE_SENTINEL_DO_NOT_EMIT")
        message(FATAL_ERROR "Console leaked workflow values")
    endif()
elseif(TEST_CASE STREQUAL "help")
    if(NOT output MATCHES "offline inspection")
        message(FATAL_ERROR "Missing offline command help")
    endif()
elseif(NOT output STREQUAL "")
    message(FATAL_ERROR "Refused inspection emitted a success payload")
endif()
if(DEFINED COMPARE_EXE)
    execute_process(COMMAND "${COMPARE_EXE}" ${args}
        RESULT_VARIABLE other_result OUTPUT_VARIABLE other_output ERROR_VARIABLE other_error TIMEOUT 35)
    if(NOT other_result STREQUAL result OR NOT other_output STREQUAL output OR NOT other_error STREQUAL error)
        message(FATAL_ERROR "CLI and Media shared command results differ")
    endif()
endif()
