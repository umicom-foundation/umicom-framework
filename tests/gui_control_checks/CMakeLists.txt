#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/gui_control_checks/CMakeLists.txt
# PURPOSE: Check control maps and scenario outcomes against real Framework code.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomGuiControlChecks LANGUAGES C)
    include(CTest)
    get_filename_component(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    add_library(umicom-control-check-support STATIC
        "${_framework}/src/ui/control_inventory.c"
        "${_framework}/src/ui/interaction_recording.c"
        "${_framework}/src/ui/automation.c"
        "${_framework}/src/ui/types.c"
        "${_framework}/src/base/status.c")
    target_include_directories(umicom-control-check-support PUBLIC "${_framework}/include")
    set_target_properties(umicom-control-check-support PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-control-check-support PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_check_library umicom-control-check-support)
else()
    set(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    set(_check_library Umicom::ui)
endif()
add_executable(umicom-gui-control-check-tests test_control_checks.c)
target_link_libraries(umicom-gui-control-check-tests PRIVATE ${_check_library})
set_target_properties(umicom-gui-control-check-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-gui-control-check-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
foreach(_case inventory-lifecycle invalid-capacity invalid-parent scope-boundary scope-duplicate
        malformed-fields invalid-flags private inherited duplicate-id independent-windows capacity
        snapshot-copy export-json export-html export-failure assert-text-pass assert-text-mismatch
        assert-visible assert-enabled assert-focused assert-selected invalid-step invalid-observation
        wrong-target malformed-response failed-driver wait-observation continue-stop import-assertion)
    add_test(NAME framework.gui_control_checks.${_case} COMMAND umicom-gui-control-check-tests ${_case})
    set_tests_properties(framework.gui_control_checks.${_case} PROPERTIES TIMEOUT 15 LABELS "framework;gui-control-checks;core")
endforeach()
option(UMICOM_GUI_CONTROL_NATIVE_TESTS "Require GTK4 for native control checks" OFF)
if(TARGET umicom_ui_gtk4 OR UMICOM_GUI_CONTROL_NATIVE_TESTS)
    if(TARGET umicom_ui_gtk4)
        set(_native_library Umicom::ui_gtk4)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(CONTROL_GTK REQUIRED IMPORTED_TARGET gtk4>=4.10)
        add_library(umicom-control-native-support STATIC
            "${_framework}/adapters/gtk4/automation_gtk4.c"
            "${_framework}/adapters/gtk4/interaction_recording_gtk4.c")
        target_link_libraries(umicom-control-native-support PUBLIC ${_check_library} PkgConfig::CONTROL_GTK)
        set_target_properties(umicom-control-native-support PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
        if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(umicom-control-native-support PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
        endif()
        set(_native_library umicom-control-native-support)
    endif()
    add_executable(umicom-gui-control-native-tests test_control_checks_gtk4.c)
    target_link_libraries(umicom-gui-control-native-tests PRIVATE ${_native_library})
    set_target_properties(umicom-gui-control-native-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-gui-control-native-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    foreach(_case text-view read-only inherited-disabled effective-visibility delayed-control
            recurring-idle duplicate-id isolated-root check-button private-text inventory
            inspection-export selection-boundary removed-control)
        add_test(NAME framework.gui_control_checks.gtk4.${_case} COMMAND umicom-gui-control-native-tests ${_case})
        set_tests_properties(framework.gui_control_checks.gtk4.${_case} PROPERTIES
            TIMEOUT 20 SKIP_RETURN_CODE 77 LABELS "framework;gui-control-checks;gtk4;acceptance")
    endforeach()
endif()
