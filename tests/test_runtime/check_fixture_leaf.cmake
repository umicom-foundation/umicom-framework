#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_runtime/check_fixture_leaf.cmake
# Purpose: Check nested-build path budget and independent fixture identities.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------
if(NOT DEFINED UMICOM_FRAMEWORK_SOURCE OR NOT DEFINED CASE)
    message(FATAL_ERROR "Provide UMICOM_FRAMEWORK_SOURCE and CASE")
endif()
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomTestWorkspace.cmake")
set(CMAKE_BINARY_DIR "C:/qualification/build-one")
set(test_directory "C:/source/tests/development_workflow")
set(test_name "framework.development_workflow.delivery.deploy-test-failure")
umicom_test_workspace_leaf("${test_name}" "${test_directory}" TRUE first)
if(CASE STREQUAL "ordinary")
    # Existing readable fixture paths must remain stable across this change.
    set(name "framework.sample.case")
    string(SHA256 identity "${CMAKE_BINARY_DIR}|${test_directory}|${name}")
    string(SUBSTRING "${identity}" 0 16 identity)
    umicom_test_workspace_leaf("${name}" "${test_directory}" FALSE selected)
    if(NOT selected STREQUAL "framework-sample-case-${identity}")
        message(FATAL_ERROR "Ordinary fixture location changed: ${selected}")
    endif()
elseif(CASE STREQUAL "compact")
    string(LENGTH "${first}" length)
    if(NOT length EQUAL 22 OR NOT first MATCHES "^build-[0-9a-f]+$")
        message(FATAL_ERROR "Nested build leaf is not compact: ${first}")
    endif()
    umicom_test_workspace_leaf("${test_name}" "${test_directory}" TRUE repeated)
    if(NOT first STREQUAL repeated)
        message(FATAL_ERROR "Repeated fixture selection changed identity")
    endif()
    # Include the real generated project, preset and compiler-probe suffix.
    # This is a path-budget check, not execution of the compiler or project.
    set(probe "C:/umicom/_local/test-work/${first}/Umicom Notes-1792361189-672840000-deploy-test-failure/build/windows-ucrt64-debug/CMakeFiles/CMakeScratch/TryCompile-v9fnru/CMakeFiles/cmTC_cc1e1.dir/2fec69fc.obj")
    string(LENGTH "${probe}" probe_length)
    if(probe_length GREATER 250)
        message(FATAL_ERROR "Nested fixture exhausted the object path budget: ${probe_length}")
    endif()
elseif(CASE STREQUAL "distinct-name")
    umicom_test_workspace_leaf("${test_name}-other" "${test_directory}" TRUE second)
elseif(CASE STREQUAL "distinct-directory")
    umicom_test_workspace_leaf("${test_name}" "${test_directory}/other" TRUE second)
elseif(CASE STREQUAL "distinct-build")
    set(CMAKE_BINARY_DIR "C:/qualification/build-two")
    umicom_test_workspace_leaf("${test_name}" "${test_directory}" TRUE second)
else()
    message(FATAL_ERROR "Unknown case: ${CASE}")
endif()
if(DEFINED second AND first STREQUAL second)
    message(FATAL_ERROR "Distinct fixture owners share a directory")
endif()
