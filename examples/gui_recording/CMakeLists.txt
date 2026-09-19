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
    add_test(NAME framework.gui_recording.notes-no-op COMMAND "${CMAKE_COMMAND}"
        "-DPROGRAM=$<TARGET_FILE:umicom-recording-example>" -DARGUMENT=--fail-save
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_example_failure.cmake")
    set_tests_properties(framework.gui_recording.notes-save framework.gui_recording.notes-no-op
        PROPERTIES TIMEOUT 15 LABELS "framework;gui-recording;example")
endif()

# The same UI library supplies copied control maps; no GTK dependency is needed.
add_executable(umicom-control-map-example control_checks.c)
target_link_libraries(umicom-control-map-example PRIVATE Umicom::ui)
set_target_properties(umicom-control-map-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-control-map-example PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
if(BUILD_TESTING)
    add_test(NAME framework.gui_control_checks.example COMMAND umicom-control-map-example)
    add_test(NAME framework.gui_control_checks.example-duplicate COMMAND "${CMAKE_COMMAND}"
        "-DPROGRAM=$<TARGET_FILE:umicom-control-map-example>" -DARGUMENT=--duplicate
        -P "${CMAKE_CURRENT_SOURCE_DIR}/check_example_failure.cmake")
    set_tests_properties(framework.gui_control_checks.example framework.gui_control_checks.example-duplicate
        PROPERTIES TIMEOUT 15 LABELS "framework;gui-control-checks;example")
endif()
