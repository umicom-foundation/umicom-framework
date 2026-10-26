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
