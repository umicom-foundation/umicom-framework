# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/developer_project/check_automatic_verify.cmake
#
# PURPOSE:
#   Observe native verification stages and log admission in isolated projects.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.24)
foreach(required CLI TEST_ROOT CASE GENERATOR)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing fixture argument: ${required}")
    endif()
endforeach()
set(cases bootstrap generic existing-driver reject-build reject-test reject-driver invalid-configuration empty-tests)
if(NOT CASE IN_LIST cases)
    message(FATAL_ERROR "Unknown verification fixture: ${CASE}")
endif()

# Keep every attempt for diagnosis. The command under test is outside this
# project tree, respecting Windows ownership of a running executable's image.
string(RANDOM LENGTH 16 ALPHABET 0123456789abcdef identity)
set(root "${TEST_ROOT}/run-${identity}")
if(EXISTS "${root}")
    message(FATAL_ERROR "Refuse to reuse an existing fixture: ${root}")
endif()
set(source "${root}/source")
set(build "${root}/build")
file(MAKE_DIRECTORY "${source}")
file(WRITE "${source}/CMakeLists.txt" [=[
cmake_minimum_required(VERSION 3.24)
project(AutomaticVerifyFixture LANGUAGES NONE)
file(APPEND "${CMAKE_BINARY_DIR}/events.txt" "C")
add_custom_target(stage ALL COMMAND "${CMAKE_COMMAND}"
    "-DROOT=${CMAKE_BINARY_DIR}" -P "${CMAKE_SOURCE_DIR}/build-stage.cmake" VERBATIM)
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/empty-tests")
    enable_testing()
    add_test(NAME smoke COMMAND "${CMAKE_COMMAND}"
        "-DROOT=${CMAKE_BINARY_DIR}" -P "${CMAKE_SOURCE_DIR}/test-stage.cmake")
endif()
]=])
file(WRITE "${source}/build-stage.cmake" [=[
file(APPEND "${ROOT}/events.txt" "B")
if(EXISTS "${ROOT}/reject-build")
    message(FATAL_ERROR "Intentional build rejection")
endif()
if(EXISTS "${ROOT}/generate-driver")
    file(MAKE_DIRECTORY "${ROOT}/umicom")
    configure_file("${CMAKE_CURRENT_LIST_DIR}/driver.cmake"
        "${ROOT}/umicom/automatic-tests-Debug.cmake" COPYONLY)
endif()
]=])
file(WRITE "${source}/test-stage.cmake" [=[
file(APPEND "${ROOT}/events.txt" "T")
if(EXISTS "${ROOT}/reject-test")
    message(FATAL_ERROR "Intentional test rejection")
endif()
]=])
file(WRITE "${source}/driver.cmake" [=[
get_filename_component(ROOT "${CMAKE_CURRENT_LIST_DIR}" DIRECTORY)
file(APPEND "${ROOT}/events.txt" "D")
if(EXISTS "${ROOT}/reject-driver")
    message(FATAL_ERROR "Intentional driver rejection")
endif()
]=])
if(CASE STREQUAL "empty-tests")
    file(WRITE "${source}/empty-tests" "fixture\n")
endif()
set(generator_options -G "${GENERATOR}")
if(DEFINED MAKE_PROGRAM AND NOT MAKE_PROGRAM STREQUAL "")
    list(APPEND generator_options "-DCMAKE_MAKE_PROGRAM:FILEPATH=${MAKE_PROGRAM}")
endif()
if(DEFINED GENERATOR_PLATFORM AND NOT GENERATOR_PLATFORM STREQUAL "")
    list(APPEND generator_options -A "${GENERATOR_PLATFORM}")
endif()
if(DEFINED GENERATOR_TOOLSET AND NOT GENERATOR_TOOLSET STREQUAL "")
    list(APPEND generator_options -T "${GENERATOR_TOOLSET}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${source}" -B "${build}"
    ${generator_options} RESULT_VARIABLE configured
    OUTPUT_FILE "${root}/configure.stdout.txt" ERROR_FILE "${root}/configure.stderr.txt" TIMEOUT 60)
if(NOT configured STREQUAL "0")
    message(FATAL_ERROR "Fixture configuration failed (${configured}); inspect ${root}")
endif()
if(CASE STREQUAL "existing-driver" OR CASE STREQUAL "reject-driver")
    file(MAKE_DIRECTORY "${build}/umicom")
    configure_file("${source}/driver.cmake" "${build}/umicom/automatic-tests-Debug.cmake" COPYONLY)
elseif(CASE STREQUAL "bootstrap")
    file(WRITE "${build}/generate-driver" "fixture\n")
endif()
if(CASE MATCHES "^reject-")
    file(WRITE "${build}/${CASE}" "fixture\n")
endif()
set(configuration Debug)
if(CASE STREQUAL "invalid-configuration")
    set(configuration "Debug/escape")
endif()
get_filename_component(tool_directory "${CMAKE_COMMAND}" DIRECTORY)
if(WIN32)
    set(ENV{PATH} "${tool_directory};$ENV{PATH}")
else()
    set(ENV{PATH} "${tool_directory}:$ENV{PATH}")
endif()
execute_process(COMMAND "${CLI}" verify --source "${source}" --build "${build}"
    --config "${configuration}" RESULT_VARIABLE verified
    OUTPUT_FILE "${root}/verify.stdout.txt" ERROR_FILE "${root}/verify.stderr.txt" TIMEOUT 90)
set(expected_result 0)
set(expected_events CBT)
set(expected_logs 1)
if(CASE STREQUAL "existing-driver" OR CASE STREQUAL "reject-driver")
    set(expected_events CD)
elseif(CASE STREQUAL "reject-build" OR CASE STREQUAL "empty-tests")
    set(expected_events CB)
elseif(CASE STREQUAL "invalid-configuration")
    set(expected_events C)
    set(expected_logs 0)
endif()
if(CASE MATCHES "^reject-" OR CASE STREQUAL "invalid-configuration" OR CASE STREQUAL "empty-tests")
    set(expected_result 1)
endif()
file(READ "${build}/events.txt" events)
file(GLOB logs "${build}/.umicom/verification-logs/verify-*.log")
list(LENGTH logs log_count)
if(NOT verified STREQUAL "${expected_result}" OR NOT events STREQUAL "${expected_events}" OR
   NOT log_count EQUAL expected_logs)
    message(FATAL_ERROR "Expected exit/events/logs ${expected_result}/${expected_events}/${expected_logs}; got ${verified}/${events}/${log_count}. Evidence: ${root}")
endif()
if(CASE STREQUAL "invalid-configuration" AND EXISTS "${build}/.umicom")
    message(FATAL_ERROR "Rejected admission created a log directory: ${root}")
endif()
if(CASE MATCHES "^reject-")
    foreach(log IN LISTS logs)
        file(READ "${log}" transcript)
        if(NOT transcript MATCHES "Intentional .* rejection")
            message(FATAL_ERROR "Stage failure is missing from the verification log: ${log}")
        endif()
    endforeach()
endif()
if(CASE STREQUAL "bootstrap")
    # The first Build produced a driver. Only this new request may delegate to it.
    execute_process(COMMAND "${CLI}" verify --source "${source}" --build "${build}" --config Debug
        RESULT_VARIABLE repeated OUTPUT_FILE "${root}/repeat.stdout.txt"
        ERROR_FILE "${root}/repeat.stderr.txt" TIMEOUT 90)
    file(READ "${build}/events.txt" events)
    file(GLOB logs "${build}/.umicom/verification-logs/verify-*.log")
    list(LENGTH logs log_count)
    if(NOT repeated STREQUAL "0" OR NOT events STREQUAL "CBTD" OR NOT log_count EQUAL 2)
        message(FATAL_ERROR "Next request did not select the driver once with a separate log: ${root}")
    endif()
endif()
message(STATUS "Automatic verify fixture ${CASE} completed; evidence: ${root}")
