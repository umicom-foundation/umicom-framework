#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/project_files/CMakeLists.txt
# PURPOSE: Build a project-file client against the installed Framework SDK.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNotesProjectFiles LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
find_package(UmicomFramework CONFIG REQUIRED)
add_executable(umicom-notes-project-files main.c)
target_link_libraries(umicom-notes-project-files PRIVATE Umicom::platform)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-project-files PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
include(CTest)
if(BUILD_TESTING)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/practice")
    add_test(NAME notes.project_files.create-and-index COMMAND umicom-notes-project-files --self-test)
    set_tests_properties(notes.project_files.create-and-index PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/practice")
endif()
