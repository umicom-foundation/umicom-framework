#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/native_process/tree_project/CMakeLists.txt
# PURPOSE: Make CTest the real parent of a native descendant fixture.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.21)
project(UmicomCTestProcessTree NONE)
enable_testing()
if(NOT EXISTS "${UMICOM_PROCESS_FIXTURE}")
    message(FATAL_ERROR "The native process fixture must be built before configuration.")
endif()
add_test(NAME notes.helper-tree COMMAND "${UMICOM_PROCESS_FIXTURE}" tree)
set_tests_properties(notes.helper-tree PROPERTIES TIMEOUT 20)
