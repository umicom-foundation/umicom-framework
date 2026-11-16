# Umicom Framework
# File: tests/media_generation/check_comfy_system_command.cmake
#
# PURPOSE:
#   Verify exact captured report identity and consistent command behavior across native hosts.
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
include("${CMAKE_CURRENT_LIST_DIR}/comfy_system_fixture.cmake")
umicom_comfy_system_fixture("${TEST_ROOT}" "${TEST_CASE}")
file(SHA256 "${SYSTEM_DOCUMENT_FILE}" document_before)
file(SHA256 "${SYSTEM_DATA_FILE}" system_before)
set(args comfy system "${SYSTEM_DOCUMENT}" "${SYSTEM_INPUT}")
set(expected 0)
if(TEST_CASE STREQUAL "relative")
    set(args comfy system "${SYSTEM_DOCUMENT}" relative.json)
    set(expected 1)
elseif(TEST_CASE STREQUAL "help")
    set(args comfy system --help)
elseif(TEST_CASE STREQUAL "usage")
    set(args comfy system)
    set(expected 2)
elseif(TEST_CASE STREQUAL "extra")
    list(APPEND args extra)
    set(expected 2)
elseif(TEST_CASE MATCHES "^(disabled|malformed-document|malformed-system|missing-file|directory|system-limit|document-limit|empty-file)$")
    set(expected 1)
endif()
execute_process(COMMAND "${HOST_EXE}" ${args}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 35)
if(NOT result STREQUAL "${expected}")
    message(FATAL_ERROR "System command ${TEST_CASE}: ${result}, expected ${expected}\n${output}\n${error}")
endif()
if(output MATCHES "PRIVATE_|127\\.0\\.0\\.1" OR error MATCHES "PRIVATE_|127\\.0\\.0\\.1")
    message(FATAL_ERROR "Raw argv, extension values or endpoint escaped into report output")
endif()
if(expected EQUAL 0 AND NOT TEST_CASE STREQUAL "help")
    string(JSON origin GET "${output}" origin)
    string(JSON scope GET "${output}" report scope)
    string(JSON revision_type TYPE "${output}" report connection_revision)
    string(JSON revision GET "${output}" report connection_revision)
    string(JSON digest GET "${output}" report response_sha256)
    string(JSON count GET "${output}" report device_count)
    string(JSON devices LENGTH "${output}" report devices)
    if(NOT origin STREQUAL "offline-files" OR NOT scope STREQUAL "system-observations" OR
       NOT revision_type STREQUAL "STRING" OR NOT revision STREQUAL "7" OR
       NOT digest STREQUAL system_before OR NOT count EQUAL devices)
        message(FATAL_ERROR "Captured report identity or device count changed")
    endif()
    if(TEST_CASE STREQUAL "empty")
        string(JSON ram_type TYPE "${output}" report ram_total)
        if(NOT count EQUAL 0 OR NOT ram_type STREQUAL "NULL")
            message(FATAL_ERROR "Empty report invented devices or memory")
        endif()
    else()
        string(JSON ram_type TYPE "${output}" report ram_free)
        string(JSON ram GET "${output}" report ram_free)
        string(JSON total GET "${output}" report ram_total)
        string(JSON cpu_index TYPE "${output}" report devices 0 index)
        string(JSON gpu_index TYPE "${output}" report devices 1 index)
        string(JSON gpu_value GET "${output}" report devices 1 index)
        string(JSON gpu_free GET "${output}" report devices 1 vram_free)
        string(JSON inconsistent GET "${output}" report devices 1 vram_inconsistent)
        string(JSON torch_inconsistent GET "${output}" report devices 1 torch_vram_inconsistent)
        string(JSON embedded_type TYPE "${output}" report embedded_python)
        string(JSON embedded GET "${output}" report embedded_python)
        string(JSON version_type TYPE "${output}" report python_version)
        if(NOT count EQUAL 2 OR NOT ram_type STREQUAL "STRING" OR
           NOT ram STREQUAL "9007199254740993" OR NOT total STREQUAL "9223372036854775807" OR
           NOT cpu_index STREQUAL "NULL" OR NOT gpu_index STREQUAL "STRING" OR NOT gpu_value STREQUAL "0" OR
           NOT gpu_free STREQUAL "9" OR NOT inconsistent OR torch_inconsistent OR
           NOT embedded_type STREQUAL "BOOLEAN" OR embedded OR NOT version_type STREQUAL "NULL")
            message(FATAL_ERROR "Unknown, exact-counter or inconsistency semantics changed")
        endif()
    endif()
elseif(expected GREATER 0 AND NOT output STREQUAL "")
    message(FATAL_ERROR "Failed command emitted partial standard output")
elseif(TEST_CASE STREQUAL "help" AND NOT output MATCHES "comfy system CONNECTION SYSTEM_JSON")
    message(FATAL_ERROR "System command help is missing")
endif()
if(DEFINED COMPARE_EXE)
    execute_process(COMMAND "${COMPARE_EXE}" ${args}
        RESULT_VARIABLE peer_result OUTPUT_VARIABLE peer_output ERROR_VARIABLE peer_error TIMEOUT 35)
    if(NOT peer_result STREQUAL result OR NOT peer_output STREQUAL output OR NOT peer_error STREQUAL error)
        message(FATAL_ERROR "Native host system-report behavior differs")
    endif()
endif()
file(SHA256 "${SYSTEM_DOCUMENT_FILE}" document_after)
file(SHA256 "${SYSTEM_DATA_FILE}" system_after)
if(NOT document_before STREQUAL document_after OR NOT system_before STREQUAL system_after)
    message(FATAL_ERROR "System command changed its input files")
endif()
