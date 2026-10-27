#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/release_baseline/check_composition.cmake
#
# PURPOSE:
#   Keep logs and bounded deadlines for every standalone qualification stage.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------
# The original parent-only deadline interrupted healthy nested tests without
# identifying the active stage. Retain its orchestration for review.
#[=[
# Umicom Framework | Sammy Hegab, Umicom Foundation | MIT
file(MAKE_DIRECTORY "${DEST}")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${ROOT}/tools/release_baseline" -B "${DEST}" -G Ninja "-DCMAKE_C_COMPILER=${COMPILER}" -DCMAKE_BUILD_TYPE=Release -DUMICOM_RELEASE_BASELINE_RETAIN_LIFECYCLE=OFF -DUMICOM_RELEASE_BASELINE_COMPOSITION_TESTS=OFF "-DUMICOM_RELEASE_BASELINE_DEFER=${DEFER}" RESULT_VARIABLE result OUTPUT_FILE "${DEST}/configure.txt" ERROR_FILE "${DEST}/configure-errors.txt")
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Composition configure failed; see ${DEST}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${DEST}" --parallel 2 RESULT_VARIABLE result OUTPUT_FILE "${DEST}/build.txt" ERROR_FILE "${DEST}/build-errors.txt")
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Composition build failed; see ${DEST}")
endif()
execute_process(COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${DEST}" --no-tests=error --output-on-failure RESULT_VARIABLE result OUTPUT_FILE "${DEST}/tests.txt" ERROR_FILE "${DEST}/test-errors.txt")
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Composition tests failed; see ${DEST}")
endif()
]=]
cmake_minimum_required(VERSION 3.24)
include("${CMAKE_CURRENT_LIST_DIR}/../support/qualification_deadlines.cmake")
foreach(_required ROOT DEST DEFER COMPILER)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing composition input: ${_required}")
    endif()
endforeach()
file(MAKE_DIRECTORY "${DEST}")
get_filename_component(_cmake_directory "${CMAKE_COMMAND}" DIRECTORY)
find_program(_ctest NAMES ctest HINTS "${_cmake_directory}" NO_DEFAULT_PATH REQUIRED)

function(ReleaseCompositionStep stage errors)
    umicom_qualification_stage_timeout(_deadline "${stage}")
    message(STATUS "Composition ${stage}; deadline ${_deadline}s; logs: ${DEST}")
    string(TIMESTAMP _start "%s" UTC)
    execute_process(COMMAND ${ARGN} TIMEOUT ${_deadline}
        RESULT_VARIABLE _result OUTPUT_FILE "${DEST}/${stage}.txt" ERROR_FILE "${DEST}/${errors}.txt")
    string(TIMESTAMP _finish "%s" UTC)
    math(EXPR _elapsed "${_finish}-${_start}")
    file(APPEND "${DEST}/stage-times.tsv" "${stage}\t${_elapsed}\t${_result}\tdeadline=${_deadline}\n")
    if(NOT "${_result}" STREQUAL "0")
        set(_details "")
        foreach(_log IN ITEMS "${stage}.txt" "${errors}.txt")
            file(SIZE "${DEST}/${_log}" _bytes)
            math(EXPR _offset "${_bytes} - 4096")
            if(_offset LESS 0)
                set(_offset 0)
            endif()
            file(READ "${DEST}/${_log}" _tail OFFSET ${_offset} LIMIT 4096)
            string(APPEND _details "\n${_log} tail:\n${_tail}")
        endforeach()
        message(FATAL_ERROR "Composition ${stage} failed (${_result}) after ${_elapsed}s; full logs: ${DEST}${_details}")
    endif()
    message(STATUS "Composition ${stage} finished in ${_elapsed}s")
endfunction()

ReleaseCompositionStep(configure configure-errors "${CMAKE_COMMAND}"
    -S "${ROOT}/tools/release_baseline" -B "${DEST}" -G Ninja
    "-DCMAKE_C_COMPILER=${COMPILER}" -DCMAKE_BUILD_TYPE=Release
    -DUMICOM_RELEASE_BASELINE_RETAIN_LIFECYCLE=OFF
    -DUMICOM_RELEASE_BASELINE_COMPOSITION_TESTS=OFF "-DUMICOM_RELEASE_BASELINE_DEFER=${DEFER}")
ReleaseCompositionStep(build build-errors "${CMAKE_COMMAND}" --build "${DEST}" --parallel 2)
ReleaseCompositionStep(tests test-errors "${_ctest}" --test-dir "${DEST}"
    --parallel 2 --no-tests=error --output-on-failure)
