# Umicom Framework
# File: cmake/UmicomKanbanCardDetails.cmake
# PURPOSE: Qualify revision-bound card detail edits through the shared library.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT BUILD_TESTING OR NOT TARGET Umicom::kanban)
    return()
endif()
include_guard(GLOBAL)
add_executable(umi-kanban-details-test "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban_details/test_details.c")
target_link_libraries(umi-kanban-details-test PRIVATE Umicom::kanban)
set_target_properties(umi-kanban-details-test PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
if(COMMAND umicom_apply_warnings)
    umicom_apply_warnings(umi-kanban-details-test)
endif()
if(COMMAND umicom_apply_sanitizers)
    umicom_apply_sanitizers(umi-kanban-details-test)
endif()
if(COMMAND umicom_register_validation_target)
    umicom_register_validation_target(umi-kanban-details-test)
endif()
if(COMMAND umicom_target_qualification_inputs)
    umicom_target_qualification_inputs(umi-kanban-details-test FILES
        "${CMAKE_CURRENT_LIST_DIR}/../tests/kanban/fixture.h")
endif()
foreach(_details_case IN ITEMS normal roundtrip public-privacy missing stale archived empty-title
        title-limit description-limit blocker-limit title-unclosed description-unclosed
        blocker-unclosed priority clear null actor done-blocker)
    set(_details_test "framework.kanban.details.${_details_case}")
    add_test(NAME ${_details_test} COMMAND umi-kanban-details-test ${_details_case})
    set_tests_properties(${_details_test} PROPERTIES TIMEOUT 30 LABELS "framework;kanban;card-details;regression")
    if(COMMAND umicom_test_impact_owners)
        umicom_test_impact_owners(${_details_test} TARGETS umi-kanban-details-test)
    endif()
endforeach()
unset(_details_case)
unset(_details_test)
