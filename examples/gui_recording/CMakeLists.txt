#-----------------------------------------------------------------------------
# Umicom Framework
# File: examples/gui_recording/CMakeLists.txt
# PURPOSE: Build the Notes report example using the real Framework UI library.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomRecordingExample LANGUAGES C)
    include(CTest)
    find_package(UmicomFramework CONFIG REQUIRED)
endif()
add_executable(umicom-recording-example main.c)
target_link_libraries(umicom-recording-example PRIVATE Umicom::ui)
set_target_properties(umicom-recording-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-recording-example PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.gui_recording.notes-save COMMAND umicom-recording-example)
    add_test(NAME framework.gui_recording.notes-no-op COMMAND umicom-recording-example --fail-save)
    set_tests_properties(framework.gui_recording.notes-no-op PROPERTIES WILL_FAIL TRUE)
    set_tests_properties(framework.gui_recording.notes-save framework.gui_recording.notes-no-op
        PROPERTIES TIMEOUT 15 LABELS "framework;gui-recording;example")
endif()
