#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/build_diagnostics/CMakeLists.txt
# PURPOSE: Build a diagnostic reader against the installed Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNotesDiagnostics LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-notes-diagnostics main.c)
target_link_libraries(umicom-notes-diagnostics PRIVATE Umicom::build)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-diagnostics PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME notes.build_diagnostics.preview COMMAND umicom-notes-diagnostics)
endif()
