#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/gui_recording/CMakeLists.txt
# PURPOSE: Build the real recording and automation sources for focused checks.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomGuiRecordingChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    add_library(umicom-recording-check-support STATIC
        "${_framework}/src/ui/interaction_recording.c"
        "${_framework}/src/ui/automation.c"
        "${_framework}/src/ui/types.c"
        "${_framework}/src/base/status.c")
    target_include_directories(umicom-recording-check-support PUBLIC "${_framework}/include")
    set_target_properties(umicom-recording-check-support PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    set(_recording_library umicom-recording-check-support)
else()
    set(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    set(_recording_library Umicom::ui)
endif()
add_executable(umicom-ui-recording-tests test_recording.c)
target_link_libraries(umicom-ui-recording-tests PRIVATE ${_recording_library})
set_target_properties(umicom-ui-recording-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-ui-recording-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
foreach(_case lifecycle invalid-create capacity copy timestamp unterminated utf8 enums
        attachments privacy escaping sink-failure streaming acceptance-empty acceptance-no-op acceptance-pass)
    add_test(NAME framework.gui_recording.${_case} COMMAND umicom-ui-recording-tests ${_case})
    set_tests_properties(framework.gui_recording.${_case} PROPERTIES TIMEOUT 15 LABELS "framework;gui-recording;core")
endforeach()
option(UMICOM_GUI_RECORDING_NATIVE_TESTS "Require GTK4 and run the native recorder fixture" OFF)
if(TARGET umicom_ui_gtk4 OR UMICOM_GUI_RECORDING_NATIVE_TESTS)
    if(TARGET umicom_ui_gtk4)
        set(_native_library Umicom::ui_gtk4)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(RECORDING_GTK REQUIRED IMPORTED_TARGET gtk4>=4.10)
        add_library(umicom-recording-native-check-support STATIC
            "${_framework}/adapters/gtk4/interaction_recording_gtk4.c"
            "${_framework}/adapters/gtk4/automation_gtk4.c")
        target_link_libraries(umicom-recording-native-check-support PUBLIC ${_recording_library} PkgConfig::RECORDING_GTK)
        set_target_properties(umicom-recording-native-check-support PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
        set(_native_library umicom-recording-native-check-support)
    endif()
    add_executable(umicom-ui-recording-gtk4-tests test_recording_gtk4.c)
    target_link_libraries(umicom-ui-recording-gtk4-tests PRIVATE ${_native_library})
    set_target_properties(umicom-ui-recording-gtk4-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    add_test(NAME framework.gui_recording.gtk4 COMMAND umicom-ui-recording-gtk4-tests)
    set_tests_properties(framework.gui_recording.gtk4 PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77 LABELS "framework;gtk4;gui-recording;acceptance")
endif()
