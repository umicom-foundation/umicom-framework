# Umicom Framework
# File: cmake/UmicomKanbanCompletion.cmake
# PURPOSE: Register shared completion review and private archive regression cases.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
if(NOT BUILD_TESTING OR NOT TARGET Umicom::kanban)
    return()
endif()
function(umicom_completion_group group)
    set(target "umi-kanban-completion-${group}-test")
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_completion/test_${group}.c")
    target_link_libraries(${target} PRIVATE Umicom::kanban)
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    if(COMMAND umicom_apply_warnings)
        umicom_apply_warnings(${target})
    endif()
    if(COMMAND umicom_apply_sanitizers)
        umicom_apply_sanitizers(${target})
    endif()
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_completion/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/base/value_archive_internal.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.kanban.completion.${group}.${case}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 30 LABELS "framework;kanban;completion-review;regression")
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_completion_group(capture normal owned after-destroy missing archived duplicate stale
    invalid-hash missing-hash exception exception-no-reason invalid-decision null oversized empty
    invalid-reviewer source-empty config-empty invalid-utf8 unterminated wrong-column
    complete-checklist pending-checklist blocked dependency transition already-done)
umicom_completion_group(apply normal batch stale changed-card different-content different-board
    dependency-batch exception exception-blocked replay null activity)
umicom_completion_group(archive roundtrip measure short null truncated checksum schema trailing
    embedded-nul invalid-sha invalid-decision owned unused-tail stale-reopen oversized)

umicom_completion_group(privacy board-archive public-html private-archive)
