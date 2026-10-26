# Umicom Framework
# File: cmake/UmicomKanbanCompletionNative.cmake
# PURPOSE: Register private native review and embedded-board lifecycle regressions.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT BUILD_TESTING OR NOT TARGET Umicom::kanban_gtk4)
    return()
endif()
include_guard(GLOBAL)
foreach(group IN ITEMS native board)
    if(group STREQUAL "native")
        set(source test_native.c)
        set(cases normal no-approval load-only stale denied busy retired capture-retire
            close-load hide-show captured-path captured-fields existing invalid-file discard
            exception reentry replay retained preview-interruption approval-interruption
            status-interruption busy-load null)
    else()
        set(source test_board_native.c)
        set(cases normal no-selection selection-change external external-refresh
            stale-selection new-selection busy retired busy-after-save retire-after-save
            blocked blocked-exception load-only different-board retained-child)
    endif()
    set(target "umi-kanban-completion-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/${source}")
    target_link_libraries(${target} PRIVATE Umicom::kanban_gtk4)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/native_fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/build_log/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
    endif()
    foreach(case IN LISTS cases)
        set(test "framework.kanban.completion.${group}.${case}")
        set(work "${CMAKE_CURRENT_BINARY_DIR}/kanban-completion-native/${group}/${case}")
        file(MAKE_DIRECTORY "${work}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES WORKING_DIRECTORY "${work}" TIMEOUT 45
            SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;kanban;gtk4;completion-review;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target} umicom_kanban_gtk4)
        endif()
    endforeach()
endforeach()
unset(group)
unset(source)
unset(cases)
unset(target)
unset(case)
unset(test)
unset(work)

# Exercise report integration only when the optional native provider is composed.
get_target_property(_completion_report_provider umicom_ui_gtk4 UMICOM_CTEST_REPORT_BROWSER_AVAILABLE)
if(_completion_report_provider)
    add_executable(umi-kanban-completion-report-test
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/test_report_native.c")
    target_link_libraries(umi-kanban-completion-report-test PRIVATE Umicom::kanban_gtk4)
    set_target_properties(umi-kanban-completion-report-test PROPERTIES
        C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(umi-kanban-completion-report-test)
    umicom_apply_sanitizers(umi-kanban-completion-report-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umi-kanban-completion-report-test)
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(umi-kanban-completion-report-test FILES
            "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/native_fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/build_log/fixture.h"
            "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
    endif()
    foreach(case IN ITEMS normal mixed empty-suite no-report report-busy file-change
            draft-only approval-revoked hidden retired source-fields)
        set(test "framework.kanban.completion.report.${case}")
        set(work "${CMAKE_CURRENT_BINARY_DIR}/kanban-completion-report/${case}")
        file(MAKE_DIRECTORY "${work}")
        add_test(NAME ${test} COMMAND umi-kanban-completion-report-test ${case})
        set_tests_properties(${test} PROPERTIES WORKING_DIRECTORY "${work}" TIMEOUT 45
            SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
            LABELS "framework;kanban;gtk4;saved-report;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS umi-kanban-completion-report-test umicom_kanban_gtk4 umicom_ui_gtk4)
        endif()
    endforeach()
endif()
unset(_completion_report_provider)
unset(case)
unset(test)
unset(work)

# Close/release checks intentionally leave only worker-owned root references.
add_executable(umi-kanban-async-owner-test
    "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/test_async_owner.c")
target_link_libraries(umi-kanban-async-owner-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-async-owner-test PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-kanban-async-owner-test)
umicom_apply_sanitizers(umi-kanban-async-owner-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-async-owner-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-async-owner-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_publication_plan/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/build_log/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(case IN ITEMS completion-load completion-save completion-missing
        publication-load publication-save publication-missing)
    set(test "framework.kanban.async_owner.${case}")
    set(work "${CMAKE_CURRENT_BINARY_DIR}/kanban-async-owner/${case}")
    file(MAKE_DIRECTORY "${work}")
    add_test(NAME ${test} COMMAND umi-kanban-async-owner-test ${case})
    set_tests_properties(${test} PROPERTIES WORKING_DIRECTORY "${work}" TIMEOUT 45
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;gtk4;lifetime;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-kanban-async-owner-test umicom_kanban_gtk4)
    endif()
endforeach()
unset(case)
unset(test)
unset(work)
