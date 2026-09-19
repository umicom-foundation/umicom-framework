#-----------------------------------------------------------------------------
# Umicom Framework
# File: tests/gui_check_runs/CMakeLists.txt
# PURPOSE: Test check sessions against the production UI implementation.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
#-----------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.24)
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(UmicomGuiCheckRuns LANGUAGES C)
    include(CTest)
    get_filename_component(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
    add_library(umicom-check-run-support STATIC
        "${_framework}/src/ui/automation.c"
        "${_framework}/src/ui/automation_report.c"
        "${_framework}/src/ui/types.c"
        "${_framework}/src/base/status.c")
    target_include_directories(umicom-check-run-support PUBLIC "${_framework}/include")
    set_target_properties(umicom-check-run-support PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-check-run-support PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    set(_check_library umicom-check-run-support)
else()
    set(_framework "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    set(_check_library Umicom::ui)
endif()
add_executable(umicom-gui-check-run-tests test_sessions.c)
target_link_libraries(umicom-gui-check-run-tests PRIVATE ${_check_library})
set_target_properties(umicom-gui-check-run-tests PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(umicom-gui-check-run-tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()
foreach(_case ready invalid empty paced complete copied-plan captured-policy capacity
    stop continue contradiction cancel-before cancel-between cancel-callback cancel-last
    reentry terminal partial-count row-bounds legacy legacy-stop legacy-empty
    export-json export-html export-junit junit-cancel redaction escaping export-destroy
    export-failure export-invalid malformed-export xml-characters state-matrix)
    add_test(NAME framework.gui_check_runs.${_case} COMMAND umicom-gui-check-run-tests ${_case})
    set_tests_properties(framework.gui_check_runs.${_case} PROPERTIES TIMEOUT 15 LABELS "framework;gui-check-runs;core")
endforeach()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umicom-gui-check-run-tests)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umicom-gui-check-run-tests)
endif()

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    add_executable(umicom-check-session-example "${_framework}/examples/gui_recording/check_sessions.c")
    target_link_libraries(umicom-check-session-example PRIVATE ${_check_library})
    set_target_properties(umicom-check-session-example PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(umicom-check-session-example PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    endif()
    foreach(_mode IN ITEMS pass fail cancel)
        add_test(NAME framework.gui_check_runs.notes-${_mode} COMMAND "${CMAKE_COMMAND}"
            "-DPROGRAM=$<TARGET_FILE:umicom-check-session-example>" "-DMODE=${_mode}"
            -P "${_framework}/examples/gui_recording/check_session_result.cmake")
        set_tests_properties(framework.gui_check_runs.notes-${_mode} PROPERTIES
            TIMEOUT 15 LABELS "framework;gui-check-runs;example")
    endforeach()
endif()

# Linker fault injection is a Linux test, not a claimed Windows heap test.
if(CMAKE_SYSTEM_NAME STREQUAL "Linux" AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_executable(umicom-check-run-allocation-test test_allocation.c)
    target_link_libraries(umicom-check-run-allocation-test PRIVATE ${_check_library})
    target_link_options(umicom-check-run-allocation-test PRIVATE "-Wl,--wrap=calloc")
    target_compile_options(umicom-check-run-allocation-test PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
    set_target_properties(umicom-check-run-allocation-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED ON)
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(umicom-check-run-allocation-test)
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-check-run-allocation-test)
    endif()
    add_test(NAME framework.gui_check_runs.allocation COMMAND umicom-check-run-allocation-test)
    set_tests_properties(framework.gui_check_runs.allocation PROPERTIES TIMEOUT 15 LABELS "framework;gui-check-runs;fault-injection")
endif()
