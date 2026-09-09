#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/cmake/test_build_contract/run_fixture.cmake
# PURPOSE: Check expected configure failures and real header compilation.
# AUTHOR: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
foreach(required FRAMEWORK_ROOT FIXTURE_BINARY_DIR CONTRACT_CASE TEST_COMPILER TEST_GENERATOR)
    if(NOT DEFINED ${required} OR "${${required}}" STREQUAL "")
        message(FATAL_ERROR "Missing fixture input: ${required}")
    endif()
endforeach()
if(NOT DEFINED TEST_CONFIG OR TEST_CONFIG STREQUAL "")
    set(TEST_CONFIG Debug)
endif()
file(MAKE_DIRECTORY "${FIXTURE_BINARY_DIR}")
set(_configure "${CMAKE_COMMAND}"
    -S "${CMAKE_CURRENT_LIST_DIR}/fixture" -B "${FIXTURE_BINARY_DIR}"
    -G "${TEST_GENERATOR}"
    "-DCMAKE_C_COMPILER=${TEST_COMPILER}"
    "-DCMAKE_BUILD_TYPE=${TEST_CONFIG}"
    "-DFRAMEWORK_ROOT=${FRAMEWORK_ROOT}"
    "-DCONTRACT_CASE=${CONTRACT_CASE}")
if(DEFINED TEST_MAKE_PROGRAM AND NOT TEST_MAKE_PROGRAM STREQUAL "")
    list(APPEND _configure "-DCMAKE_MAKE_PROGRAM=${TEST_MAKE_PROGRAM}")
endif()
if(DEFINED TEST_PLATFORM AND NOT TEST_PLATFORM STREQUAL "")
    list(APPEND _configure -A "${TEST_PLATFORM}")
endif()
if(DEFINED TEST_TOOLSET AND NOT TEST_TOOLSET STREQUAL "")
    list(APPEND _configure -T "${TEST_TOOLSET}")
endif()
execute_process(COMMAND ${_configure}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 90)
file(WRITE "${FIXTURE_BINARY_DIR}/configure.log" "${output}\n${error}")

set(_diagnostic "")
if(CONTRACT_CASE STREQUAL "missing-link")
    set(_diagnostic "must link Umicom::base")
elseif(CONTRACT_CASE STREQUAL "missing-header")
    set(_diagnostic "missing Framework header")
elseif(CONTRACT_CASE STREQUAL "missing-sdk")
    set(_diagnostic "Umicom::base is unavailable")
elseif(CONTRACT_CASE STREQUAL "missing-target")
    set(_diagnostic "does not exist")
elseif(CONTRACT_CASE STREQUAL "non-executable")
    set(_diagnostic "must be a local executable")
endif()
if(NOT _diagnostic STREQUAL "")
    if("${result}" STREQUAL "0" OR NOT "${output}\n${error}" MATCHES "${_diagnostic}")
        message(FATAL_ERROR "Expected '${_diagnostic}', got ${result}:\n${output}\n${error}")
    endif()
    message(STATUS "Rejected ${CONTRACT_CASE} before building: ${_diagnostic}")
    return()
endif()
if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "Fixture configuration failed (${result}):\n${output}\n${error}")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${FIXTURE_BINARY_DIR}"
    --config "${TEST_CONFIG}" --parallel 2 --verbose
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 90)
file(WRITE "${FIXTURE_BINARY_DIR}/build.log" "${output}\n${error}")
if(CONTRACT_CASE STREQUAL "bypass-guard")
    # Demonstrate that removing both the dependency and the guard recreates
    # the compiler failure. A random configure/link failure is not a pass.
    if("${result}" STREQUAL "0" OR
       NOT "${output}\n${error}" MATCHES "umicom/test_runtime/check[.]h")
        message(FATAL_ERROR "Expected the original missing-header compile failure:\n${output}\n${error}")
    endif()
    message(STATUS "Reproduced missing-header build failure when the guard is bypassed.")
    return()
endif()
if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "Valid fixture failed to build (${result}):\n${output}\n${error}")
endif()
execute_process(COMMAND "${CMAKE_CTEST_COMMAND}" --test-dir "${FIXTURE_BINARY_DIR}"
    -C "${TEST_CONFIG}" --output-on-failure --no-tests=error
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 30)
file(WRITE "${FIXTURE_BINARY_DIR}/ctest.log" "${output}\n${error}")
if(NOT "${result}" STREQUAL "0")
    message(FATAL_ERROR "Valid fixture test failed (${result}):\n${output}\n${error}")
endif()
message(STATUS "${CONTRACT_CASE}: configured, compiled, linked and executed the real header consumer.")
