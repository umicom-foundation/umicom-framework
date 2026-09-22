#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/native_recovery/CMakeLists.txt
# PURPOSE: Independently validate evidence and GtkSourceView data contracts.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
project(UmicomNativeRecoveryChecks LANGUAGES NONE)
include(CTest)
find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
get_filename_component(FRAMEWORK "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
add_test(NAME framework.native_recovery.evidence-gate
    COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/test_acceptance_gate.py")
set_tests_properties(framework.native_recovery.evidence-gate PROPERTIES TIMEOUT 60)
foreach(CASE IN ITEMS no-sourceview wrong-major complete optional-data uppercase msvc-name
        missing-schema directory-schema empty-schema)
    set(_driver test_sourceview_resources.cmake)
    if(CASE MATCHES "^(missing|directory|empty)-schema$")
        set(_driver expect_sourceview_refusal.cmake)
    endif()
    add_test(NAME framework.native_recovery.sourceview.${CASE}
        COMMAND "${CMAKE_COMMAND}" "-DCASE=${CASE}" "-DFRAMEWORK=${FRAMEWORK}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/cases/${CASE}"
            -P "${CMAKE_CURRENT_LIST_DIR}/${_driver}")
endforeach()
if(UNIX)
    add_test(NAME framework.native_recovery.sourceview.schema-symlink
        COMMAND "${CMAKE_COMMAND}" -DCASE=schema-symlink "-DFRAMEWORK=${FRAMEWORK}"
            "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/cases/schema-symlink"
            -P "${CMAKE_CURRENT_LIST_DIR}/expect_sourceview_refusal.cmake")
endif()
# GTK is opt-in for this portable utility project, but a requested native build
# is required to find GTK4 and a usable display. It never substitutes GTK3.
option(UMICOM_NATIVE_RECOVERY_BUILD_GTK_TESTS "Build actual GTK4 buffer recovery checks" OFF)
if(UMICOM_NATIVE_RECOVERY_BUILD_GTK_TESTS)
    enable_language(C)
    set(CMAKE_C_STANDARD 23)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4>=4.10)
    add_executable(umicom-native-buffer-recovery-test test_editor_buffer_replace.c)
    target_link_libraries(umicom-native-buffer-recovery-test PRIVATE PkgConfig::GTK4)
    if(MSVC)
        target_compile_options(umicom-native-buffer-recovery-test PRIVATE /W4 /WX)
    else()
        target_compile_options(umicom-native-buffer-recovery-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    add_test(NAME framework.native_recovery.editor.grouped-rollback
        COMMAND umicom-native-buffer-recovery-test)
    set_tests_properties(framework.native_recovery.editor.grouped-rollback PROPERTIES
        TIMEOUT 20 ENVIRONMENT "G_DEBUG=fatal-warnings;GTK_A11Y=test")
endif()
