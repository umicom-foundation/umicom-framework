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

# Read-only background refresh example; it shares the same installed API.
add_executable(umicom-notes-file-refresh refresh.c)
target_link_libraries(umicom-notes-file-refresh PRIVATE Umicom::platform)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-file-refresh PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/refresh-practice")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/refresh-practice/notes.c" "/* Umicom Notes source. */\n")
    add_test(NAME notes.project_files.background-refresh
        COMMAND umicom-notes-file-refresh "${CMAKE_CURRENT_BINARY_DIR}/refresh-practice")
    set_tests_properties(notes.project_files.background-refresh PROPERTIES
        TIMEOUT 45 PASS_REGULAR_EXPRESSION "Indexed files: 1")
endif()

# Find saved source text using the same Framework session consumed by Studio.
add_executable(umicom-notes-file-search search.c)
target_link_libraries(umicom-notes-file-search PRIVATE Umicom::platform)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-file-search PRIVATE
        -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/search-practice")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/search-practice/notes.c"
        "/* Umicom Notes */\nunsigned savedCount = 2U;\n/* savedCount tracks saved notes. */\n")
    add_test(NAME notes.project_files.find-in-files COMMAND umicom-notes-file-search
        "${CMAKE_CURRENT_BINARY_DIR}/search-practice" savedCount)
    set_tests_properties(notes.project_files.find-in-files PROPERTIES
        TIMEOUT 30 PASS_REGULAR_EXPRESSION "Matches: 2")
endif()
