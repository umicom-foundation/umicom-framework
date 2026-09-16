#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/editor_workflow/CMakeLists.txt
# PURPOSE: Build the Notes search example against an installed Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNotesSearch LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-notes-search main.c)
target_link_libraries(umicom-notes-search PRIVATE Umicom::editor)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-search PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    add_test(NAME notes.search.preview COMMAND umicom-notes-search)
endif()
install(TARGETS umicom-notes-search RUNTIME DESTINATION bin)
