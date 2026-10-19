#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_impact/check_test_graph.cmake
# Purpose: Retain diagnostics for isolated CTest ownership configuration cases.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED ROOT OR NOT DEFINED DEST OR NOT DEFINED CASE)
    message(FATAL_ERROR "ROOT, DEST and CASE are required")
endif()
# A new leaf prevents stale registrations or cached cases from passing a retry.
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef _run)
set(_build "${DEST}/${_run}")
file(MAKE_DIRECTORY "${_build}")
execute_process(COMMAND "${CMAKE_COMMAND}"
    -S "${ROOT}/tests/release_impact/test_graph_fixture" -B "${_build}" -G Ninja
    "-DROOT=${ROOT}" "-DCASE=${CASE}"
    RESULT_VARIABLE _result OUTPUT_FILE "${_build}/configure.txt"
    ERROR_FILE "${_build}/configure-errors.txt" TIMEOUT 90)
if(NOT _result STREQUAL "0")
    file(READ "${_build}/configure-errors.txt" _errors LIMIT 8192)
    message(FATAL_ERROR "Test ownership case failed (${_result}); see ${_build}\n${_errors}")
endif()
if(NOT EXISTS "${_build}/graph-evidence.txt")
    message(FATAL_ERROR "Test ownership assertions produced no evidence: ${_build}")
endif()
message(STATUS "Retained test ownership evidence: ${_build}/graph-evidence.txt")

if(DEFINED TOOL)
    file(SHA256 "${_build}/inventory.tsv" _before)
    execute_process(COMMAND "${TOOL}" impact --inventory "${_build}/inventory.tsv"
        --changed alpha.owner.c --expression
        RESULT_VARIABLE _preview_result OUTPUT_VARIABLE _preview ERROR_VARIABLE _errors TIMEOUT 30)
    file(WRITE "${_build}/preview.txt" "${_preview}")
    file(WRITE "${_build}/preview-errors.txt" "${_errors}")
    file(SHA256 "${_build}/inventory.tsv" _after)
    if(NOT _preview_result STREQUAL "0" OR NOT _before STREQUAL _after)
        message(FATAL_ERROR "Native graph preview failed or changed its input: ${_build}")
    endif()
    if(CASE STREQUAL "dependency")
        set(_expected alpha beta)
    elseif(CASE STREQUAL "fixture")
        set(_expected alpha beta cleanup)
    elseif(CASE STREQUAL "cross-directory")
        set(_expected alpha child.check cleanup)
    elseif(CASE STREQUAL "missing-dependency")
        set(_expected alpha beta cleanup outsider)
        if(NOT _preview MATCHES "Full selection:" OR NOT _preview MATCHES "No narrow expression emitted")
            message(FATAL_ERROR "Incomplete graph did not widen native selection")
        endif()
    else()
        message(FATAL_ERROR "No independent native selection expectation for ${CASE}")
    endif()
    string(REPLACE "\n" ";" _preview_lines "${_preview}")
    set(_actual "")
    foreach(_line IN LISTS _preview_lines)
        if(_line MATCHES "^test\t([^ ]+) ")
            list(APPEND _actual "${CMAKE_MATCH_1}")
        endif()
    endforeach()
    list(SORT _expected)
    list(SORT _actual)
    if(NOT _expected STREQUAL _actual)
        message(FATAL_ERROR "Native test selection ${_actual}; expected ${_expected}. See ${_build}")
    endif()
    if(NOT CASE STREQUAL "missing-dependency" AND _preview MATCHES "Full selection:")
        message(FATAL_ERROR "Resolved graph unexpectedly widened; see ${_build}")
    endif()
endif()
