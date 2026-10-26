# Umicom Framework
# File: cmake/UmicomKanbanCompletionStorage.cmake
# PURPOSE: Register private evidence-file cases in full and focused storage builds.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
include_guard(GLOBAL)
if(NOT BUILD_TESTING OR NOT TARGET Umicom::kanban_storage)
    return()
endif()
set(target umi-kanban-completion-storage-test)
add_executable(${target} "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/test_storage.c")
target_link_libraries(${target} PRIVATE Umicom::kanban_storage)
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
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_completion/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/build_log/fixture.h"
        "${CMAKE_CURRENT_LIST_DIR}/../tests/support/symbolic_link.h")
endif()
foreach(case IN ITEMS roundtrip existing different-existing malformed truncated empty oversized
        missing missing-parent relative suffix null invalid-plan unicode owned stale board-unchanged
        symlink-load symlink-write)
    set(test "framework.kanban.completion.storage.${case}")
    set(work "${CMAKE_CURRENT_BINARY_DIR}/kanban-completion-files/${case}")
    file(MAKE_DIRECTORY "${work}")
    add_test(NAME ${test} COMMAND ${target} ${case})
    set_tests_properties(${test} PROPERTIES WORKING_DIRECTORY "${work}" TIMEOUT 30
        LABELS "framework;kanban;completion-review;storage;regression")
    if(case MATCHES "^symlink-")
        set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77)
    endif()
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS ${target})
    endif()
endforeach()
unset(target)
unset(test)
unset(work)
unset(case)

# The same consumer source also checks the in-tree public link interface.
add_executable(umi-kanban-completion-consumer-check
    "${CMAKE_CURRENT_LIST_DIR}/../examples/kanban_completion/main.c")
target_link_libraries(umi-kanban-completion-consumer-check PRIVATE Umicom::kanban_storage)
set_target_properties(umi-kanban-completion-consumer-check PROPERTIES
    C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-completion-consumer-check)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-completion-consumer-check)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-completion-consumer-check)
endif()
foreach(scenario normal blocked stale existing suffix)
    set(test "framework.kanban.completion.consumer.${scenario}")
    add_test(NAME ${test} COMMAND "${CMAKE_COMMAND}"
        "-DPROGRAM=$<TARGET_FILE:umi-kanban-completion-consumer-check>"
        "-DWORK=${CMAKE_CURRENT_BINARY_DIR}/completion-consumer/${scenario}"
        "-DSCENARIO=${scenario}"
        -P "${CMAKE_CURRENT_LIST_DIR}/../examples/kanban_completion/check.cmake")
    set_tests_properties(${test} PROPERTIES TIMEOUT 30
        LABELS "framework;kanban;sdk;public-consumer;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${test} TARGETS umi-kanban-completion-consumer-check umicom_kanban_storage)
    endif()
endforeach()
unset(scenario)
unset(test)
