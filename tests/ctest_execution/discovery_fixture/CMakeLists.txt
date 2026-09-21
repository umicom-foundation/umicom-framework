#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/ctest_execution/discovery_fixture/CMakeLists.txt
# PURPOSE: Generate disposable CTest discovery catalogues without running tests.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)
project(UmicomDiscoveryFixture NONE)
enable_testing()
if(NOT DEFINED CASE_COUNT)
    message(FATAL_ERROR "CASE_COUNT is required")
endif()
if(CASE_COUNT GREATER 0)
    foreach(index RANGE 1 ${CASE_COUNT})
        set(name "notes.case.${index}")
        if(LONG_NAMES)
            string(REPEAT "x" 105 suffix)
            string(APPEND name ".${suffix}")
        endif()
        add_test(NAME "${name}" COMMAND "${CMAKE_COMMAND}" -E true)
        if(DISABLED_CASES)
            set_tests_properties("${name}" PROPERTIES DISABLED TRUE)
        endif()
    endforeach()
endif()
