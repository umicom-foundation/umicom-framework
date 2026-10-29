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
# Refuse a stale helper before the fixture configures and builds repeatedly.
# An unknown command's exit code is not evidence of an ownership rejection.
include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/UmicomTestReceiptProtocol.cmake")
file(MAKE_DIRECTORY "${RUN_ROOT}")
umicom_check_test_receipt_protocol("${RECEIPTS}"
    "${RUN_ROOT}/${_nonce}-protocol" _protocol_error)
if(NOT _protocol_error STREQUAL "")
    message(FATAL_ERROR "${_protocol_error}")
endif()

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
# Retain the success-only verifier for review. The replacement also requires
# an expected negative run to reach CTest rather than an earlier build failure.
if(FALSE)
function(verify expected label)
    file(GLOB _old LIST_DIRECTORIES TRUE "${_home}/runs/*")
    execute_process(COMMAND "${CMAKE_COMMAND}" -DUMICOM_AUTOMATIC_CONFIGURATION=Debug ${ARGN}
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
    set(_latest_run "${_run}" PARENT_SCOPE)
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
    # Complete runtime closure is a prerequisite, not an assumption of this
    # host test. Unresolved system dependencies must select the entire fixture.
    file(STRINGS "${_run}/before.tsv.policy.txt" _policy)
    if(_policy STREQUAL "full")
        set(expected "fixture.alpha;fixture.beta;fixture.unknown")
        set(_reuse_available FALSE PARENT_SCOPE)
        message(STATUS "${label}: full fallback; acceleration not qualified on this host")
        file(READ "${_run}/reused-tests.txt" _excluded)
        if(NOT _excluded STREQUAL "")
            message(FATAL_ERROR "Incomplete runtime closure excluded a test")
        endif()
    elseif(_policy STREQUAL "eligible")
        set(_reuse_available TRUE PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Missing or invalid reuse policy evidence")
    endif()
    list(SORT _names)
    list(SORT expected)
    if(NOT _names STREQUAL expected)
        message(FATAL_ERROR "${label}: expected [${expected}], selected [${_names}]; evidence ${_run}")
    endif()
endfunction()
endif()
function(verify expected label)
    file(GLOB _old LIST_DIRECTORIES TRUE "${_home}/runs/*")
    set(_extra ${ARGN})
    set(_expect_failure FALSE)
    if("EXPECT_FAILURE" IN_LIST _extra)
        set(_expect_failure TRUE)
        list(REMOVE_ITEM _extra EXPECT_FAILURE)
    endif()
    execute_process(COMMAND "${CMAKE_COMMAND}" -DUMICOM_AUTOMATIC_CONFIGURATION=Debug ${_extra}
        -P "${_build}/umicom/automatic-tests-Debug.cmake"
        RESULT_VARIABLE _status OUTPUT_VARIABLE _output ERROR_VARIABLE _error)
    if(_expect_failure)
        if(_status STREQUAL "0" OR NOT _error MATCHES "CTest failed")
            message(FATAL_ERROR "${label}: expected aggregate CTest failure, got ${_status}:\n${_output}\n${_error}")
        endif()
    elseif(NOT _status STREQUAL "0")
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
    set(_latest_run "${_run}" PARENT_SCOPE)
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
    # Complete runtime closure is a prerequisite, not an assumption of this
    # host test. Unresolved system dependencies must select the entire fixture.
    file(STRINGS "${_run}/before.tsv.policy.txt" _policy)
    if(_policy STREQUAL "full")
        set(expected "fixture.alpha;fixture.beta;fixture.unknown")
        set(_reuse_available FALSE PARENT_SCOPE)
        message(STATUS "${label}: full fallback; acceleration not qualified on this host")
        file(READ "${_run}/reused-tests.txt" _excluded)
        if(NOT _excluded STREQUAL "")
            message(FATAL_ERROR "Incomplete runtime closure excluded a test")
        endif()
    elseif(_policy STREQUAL "eligible")
        set(_reuse_available TRUE PARENT_SCOPE)
    else()
        message(FATAL_ERROR "Missing or invalid reuse policy evidence")
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
if(_reuse_available)
file(READ "${_home}/receipts.tsv" _receipts)
string(REGEX REPLACE "(fixture.beta\t[0-9a-f]+\t)2\n" "\\1FAILED\n" _failed "${_receipts}")
string(REPLACE "FAILED\n" "3\n" _failed "${_failed}")
if(_failed STREQUAL _receipts)
    message(FATAL_ERROR "Expected a beta passing receipt")
endif()
file(WRITE "${_home}/receipts.tsv" "${_failed}")
verify("fixture.beta;fixture.unknown" saved-failure)
else()
    # The portable receipt unit test covers selective outcomes independently;
    # this branch proves the real driver preserves all tests when closure fails.
    verify("fixture.alpha;fixture.beta;fixture.unknown" repeated-runtime-fallback)
endif()
file(WRITE "${_build}/Testing/Temporary/LastTestsFailed.log" "1:fixture.alpha\n")
verify("fixture.alpha;fixture.unknown" ordinary-ctest-failure)
# A missing or corrupted state never grants permission to omit a test.
file(WRITE "${_home}/receipts.tsv" "damaged state\n")
verify("fixture.alpha;fixture.beta;fixture.unknown" corrupt-state)
verify("fixture.unknown" recovered-state)
verify("fixture.alpha;fixture.beta;fixture.unknown" forced-full -DUMICOM_AUTOMATIC_TEST_FULL=ON)
if(_reuse_available)
    message(FATAL_ERROR "Explicit full run incorrectly enabled reuse")
endif()
verify("fixture.alpha;fixture.beta;fixture.unknown" after-full)
# A registered target must be the executable that CTest actually launches.
# Alter only this private fixture's registry, retaining both real executables.
file(READ "${_source}/CMakeLists.txt" _fixture_configuration)
string(REPLACE "fixture.alpha\\t$<TARGET_FILE:fixture-alpha>"
    "fixture.alpha\\t$<TARGET_FILE:fixture-beta>" _wrong_owner "${_fixture_configuration}")
if(_wrong_owner STREQUAL _fixture_configuration)
    message(FATAL_ERROR "Could not construct the wrong-owner fixture")
endif()
file(WRITE "${_source}/CMakeLists.txt" "${_wrong_owner}")
verify("fixture.alpha;fixture.beta;fixture.unknown" wrong-executable-owner)
if(_reuse_available)
    message(FATAL_ERROR "A different discovered executable was admitted to reuse")
endif()
file(READ "${_latest_run}/discovery.json.owners.log" _owner_evidence)
if(NOT _owner_evidence MATCHES "Registered owner differs")
    message(FATAL_ERROR "Wrong-owner fixture did not reach the ownership refusal")
endif()
file(WRITE "${_source}/CMakeLists.txt" "${_fixture_configuration}")
verify("fixture.alpha;fixture.beta;fixture.unknown" restored-executable-owner)

# Exercise an actual failed child test. Starting from a damaged private receipt
# selects alpha as well as beta, so a passed alpha row from this unsuccessful
# aggregate run must not become an exclusion on the following successful retry.
file(READ "${_source}/beta.c" _beta_source)
string(REPLACE "return 0;" "return 1;" _beta_failed "${_beta_source}")
if(_beta_failed STREQUAL _beta_source)
    message(FATAL_ERROR "Could not construct a failing private test")
endif()
file(WRITE "${_source}/beta.c" "${_beta_failed}")
file(WRITE "${_home}/receipts.tsv" "damaged state before failed runner\n")
verify("fixture.alpha;fixture.beta;fixture.unknown" actual-test-failure EXPECT_FAILURE)
file(READ "${_home}/receipts.tsv" _after_failed_runner)
if(_after_failed_runner MATCHES "fixture[.]alpha\t[0-9a-f]+\t2\n")
    message(FATAL_ERROR "Unsuccessful runner granted alpha a reusable pass")
endif()
if(_reuse_available)
    if(NOT _after_failed_runner MATCHES "fixture[.]alpha\t[0-9a-f]+\t0\n"
            OR NOT _after_failed_runner MATCHES "fixture[.]beta\t[0-9a-f]+\t3\n")
        message(FATAL_ERROR "Failed runner did not retain conservative retry states")
    endif()
endif()
file(WRITE "${_source}/beta.c" "${_beta_source}")
verify("fixture.alpha;fixture.beta;fixture.unknown" after-failed-runner)
verify("fixture.unknown" after-successful-retry)

message(STATUS "Automatic driver lifecycle evidence: ${_home}")
