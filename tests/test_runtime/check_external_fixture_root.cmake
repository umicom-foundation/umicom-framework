# -----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_runtime/check_external_fixture_root.cmake
# Purpose: Preserve real Git boundaries while choosing visible test scratch roots.
# Author and Organisation: Sammy Hegab, Umicom Foundation
# Licence: MIT
# -----------------------------------------------------------------------------
if(NOT DEFINED UMICOM_FRAMEWORK_SOURCE OR NOT DEFINED CASE)
    message(FATAL_ERROR "Provide UMICOM_FRAMEWORK_SOURCE and CASE")
endif()
include("${UMICOM_FRAMEWORK_SOURCE}/cmake/UmicomTestWorkspace.cmake")
set(work "$ENV{UMICOM_TEST_WORK_DIRECTORY}")
if(work STREQUAL "" OR NOT IS_DIRECTORY "${work}")
    message(FATAL_ERROR "Run through CTest with its explicit external fixture directory")
endif()
umicom_test_repository_outer_parent("${work}" ancestor)
if(NOT ancestor STREQUAL "")
    message(FATAL_ERROR "CTest supplied a fixture inside a repository: ${work}")
endif()
file(REAL_PATH "${work}" work)
set(UMICOM_TEST_EXTERNAL_WORK_ROOT "")
set(expected "${work}/_local/test-work")
set(requested "${work}/repository/build/test-work")
file(MAKE_DIRECTORY "${requested}")
if(CASE STREQUAL "git-file")
    file(WRITE "${work}/repository/.git" "gitdir: fixture-metadata\n")
elseif(CASE STREQUAL "git-directory" OR CASE STREQUAL "nested" OR CASE STREQUAL "explicit-external" OR CASE STREQUAL "explicit-inside")
    file(MAKE_DIRECTORY "${work}/repository/.git")
    if(CASE STREQUAL "nested")
        file(WRITE "${work}/repository/build/.git" "gitdir: nested-fixture-metadata\n")
    elseif(CASE STREQUAL "explicit-external")
        set(UMICOM_TEST_EXTERNAL_WORK_ROOT "${work}/chosen-external")
        set(expected "${UMICOM_TEST_EXTERNAL_WORK_ROOT}")
    elseif(CASE STREQUAL "explicit-inside")
        # A child invocation must reject an explicit path under a .git marker.
        # The parent checks its diagnostic, so unrelated command failures cannot pass.
        execute_process(COMMAND "${CMAKE_COMMAND}"
            "-DUMICOM_FRAMEWORK_SOURCE=${UMICOM_FRAMEWORK_SOURCE}"
            "-DCASE=reject-inside" -P "${CMAKE_CURRENT_LIST_FILE}"
            RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 10)
        if("${result}" STREQUAL "0" OR NOT error MATCHES "must be outside Git checkouts")
            message(FATAL_ERROR "Expected checked root rejection: ${result}\n${output}\n${error}")
        endif()
        return()
    endif()
elseif(CASE STREQUAL "reject-inside")
    set(UMICOM_TEST_EXTERNAL_WORK_ROOT "${requested}")
elseif(CASE STREQUAL "plain")
    set(expected "${requested}")
else()
    message(FATAL_ERROR "Unknown case: ${CASE}")
endif()
umicom_test_external_workspace("${requested}" selected)
file(REAL_PATH "${expected}" expected)
if(NOT selected STREQUAL expected OR NOT IS_DIRECTORY "${selected}")
    message(FATAL_ERROR "Expected ${expected}, selected ${selected}")
endif()
umicom_test_repository_outer_parent("${selected}" ancestor)
if(NOT ancestor STREQUAL "")
    message(FATAL_ERROR "Selected root remains inside a repository")
endif()
# Repeated configuration must retain the same chosen location.
umicom_test_external_workspace("${requested}" repeated)
if(NOT repeated STREQUAL selected)
    message(FATAL_ERROR "External fixture root changed on repeated selection")
endif()
