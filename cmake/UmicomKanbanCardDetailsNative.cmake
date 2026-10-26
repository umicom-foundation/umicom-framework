# Umicom Framework
# File: cmake/UmicomKanbanCardDetailsNative.cmake
# PURPOSE: Qualify visible private card editing in shared native board hosts.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT BUILD_TESTING OR NOT TARGET Umicom::kanban_gtk4)
    return()
endif()
include_guard(GLOBAL)
add_executable(umi-kanban-details-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_native.c")
target_link_libraries(umi-kanban-details-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-details-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-kanban-details-native-test)
umicom_apply_sanitizers(umi-kanban-details-native-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-details-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-details-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_details_case IN ITEMS normal no-selection stale other-selection hidden busy
        retired empty-title oversized reentry replay load-retire load-hide selection-roundtrip)
    set(_details_test "framework.kanban.details.native.${_details_case}")
    add_test(NAME ${_details_test} COMMAND umi-kanban-details-native-test ${_details_case})
    set_tests_properties(${_details_test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
        ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;kanban;gtk4;card-details;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_details_test} TARGETS umi-kanban-details-native-test umicom_kanban_gtk4)
    endif()
endforeach()
unset(_details_case)
unset(_details_test)

add_executable(umi-kanban-checklist-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_checklist_native.c")
target_link_libraries(umi-kanban-checklist-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-checklist-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-kanban-checklist-native-test)
umicom_apply_sanitizers(umi-kanban-checklist-native-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-checklist-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-checklist-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_checklist_case IN ITEMS normal clear reentry no-selection stale selection-roundtrip
        busy hidden retired load-retire load-hide done)
    set(_checklist_test "framework.kanban.checklist.native.${_checklist_case}")
    add_test(NAME ${_checklist_test} COMMAND umi-kanban-checklist-native-test ${_checklist_case})
    set_tests_properties(${_checklist_test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
        ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;kanban;gtk4;acceptance;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_checklist_test} TARGETS umi-kanban-checklist-native-test umicom_kanban_gtk4)
    endif()
endforeach()
unset(_checklist_case)
unset(_checklist_test)

add_executable(umi-kanban-dependencies-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_dependencies_native.c")
target_link_libraries(umi-kanban-dependencies-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-dependencies-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
umicom_apply_warnings(umi-kanban-dependencies-native-test)
umicom_apply_sanitizers(umi-kanban-dependencies-native-test)
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-dependencies-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-dependencies-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_dependencies_case IN ITEMS normal remove reentry no-selection placeholder stale selection-roundtrip
        busy hidden retired load-retire load-hide cycle done)
    set(_dependencies_test "framework.kanban.dependencies.native.${_dependencies_case}")
    add_test(NAME ${_dependencies_test} COMMAND umi-kanban-dependencies-native-test ${_dependencies_case})
    set_tests_properties(${_dependencies_test} PROPERTIES TIMEOUT 30 SKIP_RETURN_CODE 77
        ENVIRONMENT "G_DEBUG=fatal-warnings" LABELS "framework;kanban;gtk4;dependencies;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_dependencies_test} TARGETS umi-kanban-dependencies-native-test umicom_kanban_gtk4)
    endif()
endforeach()
unset(_dependencies_case)
unset(_dependencies_test)

add_executable(umi-kanban-labels-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_labels_native.c")
target_link_libraries(umi-kanban-labels-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-labels-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-labels-native-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-labels-native-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-labels-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-labels-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_policy_case IN ITEMS normal clear reentry no-selection stale selection-roundtrip busy hidden retired load-retire load-hide duplicate unicode-overflow)
    set(_policy_test "framework.kanban.labels.native.${_policy_case}")
    add_test(NAME ${_policy_test} COMMAND umi-kanban-labels-native-test ${_policy_case})
    set_tests_properties(${_policy_test} PROPERTIES TIMEOUT 30
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;labels;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_policy_test} TARGETS umi-kanban-labels-native-test)
    endif()
endforeach()
unset(_policy_case)
unset(_policy_test)


add_executable(umi-kanban-column-policy-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_column_policy_native.c")
target_link_libraries(umi-kanban-column-policy-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-column-policy-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-column-policy-native-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-column-policy-native-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-column-policy-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-column-policy-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_policy_case IN ITEMS normal unlimited other-list reentry not-loaded stale selection-roundtrip busy hidden retired load-retire load-hide load-selection below negative fraction spaces over-capacity overflow blank)
    set(_policy_test "framework.kanban.column-policy.native.${_policy_case}")
    add_test(NAME ${_policy_test} COMMAND umi-kanban-column-policy-native-test ${_policy_case})
    set_tests_properties(${_policy_test} PROPERTIES TIMEOUT 30
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;column-policy;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_policy_test} TARGETS umi-kanban-column-policy-native-test)
    endif()
endforeach()
unset(_policy_case)
unset(_policy_test)

# Native UTC input is independent of machine timezone and rejects truncation.

add_executable(umi-kanban-deadline-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_deadline_native.c")
target_link_libraries(umi-kanban-deadline-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-deadline-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-deadline-native-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-deadline-native-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-deadline-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-deadline-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_policy_case IN ITEMS normal clear leap invalid-leap offset fraction junk pre-epoch epoch first-second after-2038 maximum unsupported unsupported-clear stale no-selection busy hidden retired selection-roundtrip load-hide load-retire reentry)
    set(_policy_test "framework.kanban.deadline.native.${_policy_case}")
    add_test(NAME ${_policy_test} COMMAND umi-kanban-deadline-native-test ${_policy_case})
    set_tests_properties(${_policy_test} PROPERTIES TIMEOUT 30
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;deadline;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_policy_test} TARGETS umi-kanban-deadline-native-test)
    endif()
endforeach()
unset(_policy_case)
unset(_policy_test)

# Reversible archive changes retain fields and use existing domain rules.
add_executable(umi-kanban-archive-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_archive_native.c")
target_link_libraries(umi-kanban-archive-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-archive-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-archive-native-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-archive-native-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-archive-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-archive-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_policy_case IN ITEMS archive restore roundtrip done dependency restore-wip restore-dependency empty no-selection no-review stale stale-review selection-roundtrip busy hidden retired review-retire review-hide review-selection load-retire review-reentry apply-reentry unicode reload)
    set(_policy_test "framework.kanban.archive.native.${_policy_case}")
    add_test(NAME ${_policy_test} COMMAND umi-kanban-archive-native-test ${_policy_case})
    set_tests_properties(${_policy_test} PROPERTIES TIMEOUT 30
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;archive;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_policy_test} TARGETS umi-kanban-archive-native-test)
    endif()
endforeach()
unset(_policy_case)
unset(_policy_test)

# Row actions need current presentation ownership as well as board revision.
add_executable(umi-kanban-row-authority-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_gtk4/test_row_authority.c")
target_link_libraries(umi-kanban-row-authority-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-row-authority-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-row-authority-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-row-authority-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-row-authority-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-row-authority-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_policy_case IN ITEMS retained-row retained-source detached-controller retained-target late-end refresh-cancel selection-reentry selection-feedback selection-refresh selection-retire)
    set(_policy_test "framework.kanban.row-authority.${_policy_case}")
    add_test(NAME ${_policy_test} COMMAND umi-kanban-row-authority-test ${_policy_case})
    set_tests_properties(${_policy_test} PROPERTIES TIMEOUT 30
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;ownership;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_policy_test} TARGETS umi-kanban-row-authority-test)
    endif()
endforeach()
unset(_policy_case)
unset(_policy_test)

# Native filters must preserve every board record and report full occupancy.
add_executable(umi-kanban-filter-native-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_filter_native.c")
target_link_libraries(umi-kanban-filter-native-test PRIVATE Umicom::kanban_gtk4)
set_target_properties(umi-kanban-filter-native-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-filter-native-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-filter-native-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-filter-native-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-filter-native-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/native_fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/main_context.h")
endif()
foreach(_policy_case IN ITEMS text text-case description label label-case developer developer-case developer-id developer-unicode developer-unknown developer-ambiguous priority urgent blocked overdue due-now invalid-cutoff offset-cutoff combined clear clear-draft selection feedback-reentry busy hidden retired label-overflow text-overflow invalid-priority refresh)
    set(_policy_test "framework.kanban.filter.native.${_policy_case}")
    add_test(NAME ${_policy_test} COMMAND umi-kanban-filter-native-test ${_policy_case})
    set_tests_properties(${_policy_test} PROPERTIES TIMEOUT 30
        SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings"
        LABELS "framework;kanban;filter;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_policy_test} TARGETS umi-kanban-filter-native-test)
    endif()
endforeach()
unset(_policy_case)
unset(_policy_test)
