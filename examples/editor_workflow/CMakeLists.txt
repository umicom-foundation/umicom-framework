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

# The second example uses the real coordinator rather than editing a preview.
add_executable(umicom-notes-document complete_document.c)
target_link_libraries(umicom-notes-document PRIVATE Umicom::document)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-document PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME notes.document.complete COMMAND umicom-notes-document)
endif()
install(TARGETS umicom-notes-document RUNTIME DESTINATION bin)

# Review saved content before replacing a draft, using the shared coordinator.
add_executable(umicom-notes-reload reload.c)
target_link_libraries(umicom-notes-reload PRIVATE Umicom::document)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-reload PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME notes.document.reload COMMAND umicom-notes-reload)
endif()
install(TARGETS umicom-notes-reload RUNTIME DESTINATION bin)

# Read-only comparison uses the same Framework engine as Studio's review pane.
add_executable(umicom-notes-review review.c)
target_link_libraries(umicom-notes-review PRIVATE Umicom::developer)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-review PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME notes.document.review COMMAND umicom-notes-review)
endif()
install(TARGETS umicom-notes-review RUNTIME DESTINATION bin)

# Selection edits, delayed delivery and history through the document owner.
add_executable(umicom-notes-edit-commands edit_commands.c)
target_link_libraries(umicom-notes-edit-commands PRIVATE Umicom::document)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-edit-commands PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME notes.document.edit_commands COMMAND umicom-notes-edit-commands)
endif()
install(TARGETS umicom-notes-edit-commands RUNTIME DESTINATION bin)

# Navigate without altering text, and reject input for a no-longer-active tab.
add_executable(umicom-notes-navigation navigation.c)
target_link_libraries(umicom-notes-navigation PRIVATE Umicom::document)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-navigation PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME notes.document.navigation COMMAND umicom-notes-navigation)
endif()
install(TARGETS umicom-notes-navigation RUNTIME DESTINATION bin)

# Save an existing Notes source and an untitled header through one session.
add_executable(umicom-notes-save-all save_all.c)
target_link_libraries(umicom-notes-save-all PRIVATE Umicom::document)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-notes-save-all PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
endif()
if(BUILD_TESTING)
    add_test(NAME notes.document.save_all COMMAND umicom-notes-save-all)
endif()
install(TARGETS umicom-notes-save-all RUNTIME DESTINATION bin)
