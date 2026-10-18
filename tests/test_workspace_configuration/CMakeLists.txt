#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/test_workspace_configuration/CMakeLists.txt
# PURPOSE: Register isolated configuration checks for retained test fixtures.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.28)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomTestWorkspaceChecks LANGUAGES NONE)
    include(CTest)
endif()
foreach(_case default custom relative explicit-environment explicit-modification idempotent
        collision separate-directory invalid-empty invalid-list invalid-generator)
    add_test(NAME framework.test_workspace.${_case}
        COMMAND "${CMAKE_COMMAND}"
            "-DCASE=${_case}" "-DFIXTURE_SOURCE=${CMAKE_CURRENT_SOURCE_DIR}/fixture"
            "-DFIXTURE_BUILD=${CMAKE_CURRENT_BINARY_DIR}/cases/${_case}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/check.cmake")
    set_tests_properties(framework.test_workspace.${_case} PROPERTIES
        TIMEOUT 30 LABELS "framework;cmake;test-workspace;regression")
endforeach()
