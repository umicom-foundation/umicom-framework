# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/implementation_fingerprint/check_source_delivery.cmake
# PURPOSE: Keep real testing sources visible while excluding generated output.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# -----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(argument GIT_EXECUTABLE IGNORECASE WORK_ROOT)
    if(NOT DEFINED ${argument} OR "${${argument}}" STREQUAL "")
        message(FATAL_ERROR "Missing source-delivery fixture argument: ${argument}")
    endif()
endforeach()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run)
set(work "${WORK_ROOT}/source-delivery-${run}")
file(MAKE_DIRECTORY "${work}")
file(COPY "${CMAKE_CURRENT_LIST_DIR}/../../.gitignore" DESTINATION "${work}")
execute_process(COMMAND "${GIT_EXECUTABLE}" init --quiet "${work}"
    RESULT_VARIABLE result ERROR_VARIABLE error)
if(NOT result STREQUAL "0")
    message(FATAL_ERROR "Could not initialise the isolated Git fixture: ${error}")
endif()
set(sources
    src/testing/ctest_report.inc src/testing/ctest_report_files.inc src/testing/ctest_job.inc
    include/umicom/testing/ctest_job.h examples/testing/queued_ctest.c
    examples/testing/notes_project/CMakeLists.txt examples/testing/notes_project/notes_tests.c
    tools/testing/native_acceptance.py)
set(generated Testing/TAG build/gtk4-debug/Testing/TAG build/gtk4-debug/owner.obj local.log)
foreach(path IN LISTS sources generated)
    get_filename_component(directory "${work}/${path}" DIRECTORY)
    file(MAKE_DIRECTORY "${directory}")
    file(WRITE "${work}/${path}" "fixture\n")
endforeach()
foreach(path IN LISTS sources)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${work}" -c "core.ignorecase=${IGNORECASE}"
        check-ignore --no-index -- "${path}" RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT result STREQUAL "1" OR NOT output STREQUAL "")
        message(FATAL_ERROR "Product source was ignored: ${path}: ${output}${error}")
    endif()
endforeach()
foreach(path IN LISTS generated)
    execute_process(COMMAND "${GIT_EXECUTABLE}" -C "${work}" -c "core.ignorecase=${IGNORECASE}"
        check-ignore --no-index -- "${path}" RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Generated output escaped the ignore rules: ${path}: ${output}${error}")
    endif()
endforeach()
