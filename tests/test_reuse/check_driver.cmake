#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_reuse/check_driver.cmake
#
# PURPOSE:
#   Check first runs, unchanged reuse, changed binaries and unsuccessful receipts.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.29)
foreach(_required FIXTURE_ROOT RUN_ROOT RECEIPTS RUN_SCRIPT C_COMPILER OBJDUMP RUNTIME_PLATFORM GENERATOR)
    if(NOT DEFINED ${_required} OR "${${_required}}" STREQUAL "")
        message(FATAL_ERROR "Missing fixture input: ${_required}")
    endif()
endforeach()
# Each invocation owns a fresh child directory. Never delete another run's
# build tree or evidence when preparing a regression fixture.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef _nonce)
set(_source "${RUN_ROOT}/${_nonce}/source")
set(_build "${RUN_ROOT}/${_nonce}/build")
file(MAKE_DIRECTORY "${_source}")
file(COPY "${FIXTURE_ROOT}/CMakeLists.txt" DESTINATION "${_source}")
foreach(_case alpha beta unknown)
    configure_file("${FIXTURE_ROOT}/sample.c" "${_source}/${_case}.c" COPYONLY)
endforeach()
get_filename_component(_compiler_dir "${C_COMPILER}" DIRECTORY)
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${_source}" -B "${_build}"
    -G "${GENERATOR}" "-DCMAKE_C_COMPILER=${C_COMPILER}" "-DCMAKE_BUILD_TYPE=Debug"
    "-DUMICOM_AUTOMATIC_RECEIPTS=${RECEIPTS}"
    "-DUMICOM_AUTOMATIC_SCRIPT=${RUN_SCRIPT}"
    "-DUMICOM_AUTOMATIC_COMPILER_DIR=${_compiler_dir}"
    "-DUMICOM_AUTOMATIC_OBJDUMP=${OBJDUMP}"
    "-DUMICOM_AUTOMATIC_RUNTIME_PLATFORM=${RUNTIME_PLATFORM}"
    RESULT_VARIABLE _configured OUTPUT_VARIABLE _stdout ERROR_VARIABLE _stderr)
if(NOT _configured STREQUAL "0")
    message(FATAL_ERROR "Fixture configure failed:\n${_stdout}\n${_stderr}")
endif()
set(_home "${_build}/.umicom/qualification/Debug")
function(verify expected label)
    file(GLOB _old LIST_DIRECTORIES TRUE "${_home}/runs/*")
    execute_process(COMMAND "${CMAKE_COMMAND}" -DUMICOM_AUTOMATIC_CONFIGURATION=Debug
        -P "${_build}/umicom/automatic-tests-Debug.cmake"
        RESULT_VARIABLE _status OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
    if(NOT _status STREQUAL "0")
        message(FATAL_ERROR "${label}: verification failed:\n${_output}\n${_error}")
    endif()
    file(GLOB _new LIST_DIRECTORIES TRUE "${_home}/runs/*")
    if(_old)
        list(REMOVE_ITEM _new ${_old})
    endif()
    list(LENGTH _new _runs)
    if(NOT _runs EQUAL 1)
        message(FATAL_ERROR "${label}: expected exactly one new evidence directory")
    endif()
    list(GET _new 0 _run)
    file(READ "${_run}/selected.json" _selected)
    string(JSON _count LENGTH "${_selected}" tests)
    set(_names "")
    if(_count GREATER 0)
        math(EXPR _last "${_count} - 1")
        foreach(_index RANGE 0 ${_last})
            string(JSON _name GET "${_selected}" tests ${_index} name)
            list(APPEND _names "${_name}")
        endforeach()
    endif()
    list(SORT _names)
    list(SORT expected)
    if(NOT _names STREQUAL expected)
        message(FATAL_ERROR "${label}: expected [${expected}], selected [${_names}]; evidence ${_run}")
    endif()
endfunction()
verify("fixture.alpha;fixture.beta;fixture.unknown" first-run)
verify("fixture.unknown" unchanged)
# Content changes affect only one executable. The unknown-input test always
# remains selected, even though its own program did not change.
file(READ "${_source}/alpha.c" _alpha)
string(REPLACE "Automatic qualification fixture." "Changed qualification fixture." _alpha "${_alpha}")
file(WRITE "${_source}/alpha.c" "${_alpha}")
verify("fixture.alpha;fixture.unknown" changed-executable)
# Simulate an unsuccessful saved observation and a separate normal CTest run.
# Other passing receipts must remain independent in both cases.
file(READ "${_home}/receipts.tsv" _receipts)
string(REGEX REPLACE "(fixture.beta\t[0-9a-f]+\t)2\n" "\\1FAILED\n" _failed "${_receipts}")
string(REPLACE "FAILED\n" "3\n" _failed "${_failed}")
if(_failed STREQUAL _receipts)
    message(FATAL_ERROR "Expected a beta passing receipt")
endif()
file(WRITE "${_home}/receipts.tsv" "${_failed}")
verify("fixture.beta;fixture.unknown" saved-failure)
file(WRITE "${_build}/Testing/Temporary/LastTestsFailed.log" "1:fixture.alpha\n")
verify("fixture.alpha;fixture.unknown" ordinary-ctest-failure)
# A missing or corrupted state never grants permission to omit a test.
file(WRITE "${_home}/receipts.tsv" "damaged state\n")
verify("fixture.alpha;fixture.beta;fixture.unknown" corrupt-state)
verify("fixture.unknown" recovered-state)
message(STATUS "Automatic driver lifecycle evidence: ${_home}")
