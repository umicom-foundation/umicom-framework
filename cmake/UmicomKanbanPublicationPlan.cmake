# Umicom Framework
# File: cmake/UmicomKanbanPublicationPlan.cmake
# PURPOSE: Register private publication-plan contracts as their owners become available.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
# This file may be included again after the optional native adapter is composed.
if(NOT BUILD_TESTING OR NOT TARGET Umicom::kanban)
    return()
endif()
function(umicom_public_plan_group group owner)
    set(target "umi-public-plan-${group}-test")
    if(TARGET ${target})
        return()
    endif()
    add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_publication_plan/test_${group}.c")
    target_link_libraries(${target} PRIVATE ${owner})
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_publication_plan/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/build_log/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/support/main_context.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../src/base/value_archive_internal.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.kanban.public_plan.${group}.${case}")
        set(work "${CMAKE_CURRENT_BINARY_DIR}/test-work/public-plan/${group}/${case}")
        file(MAKE_DIRECTORY "${work}")
        add_test(NAME ${test} COMMAND ${target} ${case})
        set_tests_properties(${test} PROPERTIES TIMEOUT 60 WORKING_DIRECTORY "${work}"
            LABELS "framework;kanban;publication-plan;regression")
        if(group STREQUAL "native" OR group STREQUAL "integration")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_public_plan_group(capture Umicom::kanban normal empty owned after-destroy null stale missing archived duplicate title-empty invalid-utf8 unterminated bounds private-fields unicode card-order)
umicom_public_plan_group(archive Umicom::kanban roundtrip short measure truncated checksum schema trailing embedded-nul invalid-utf8 oversized-count duplicate empty ownership unused-tail null)
umicom_public_plan_group(review Umicom::kanban current moved changed-title archived different-board stale-request empty old-unchanged render-stale render-short)
if(TARGET Umicom::kanban_storage)
    umicom_public_plan_group(storage Umicom::kanban_storage roundtrip replace missing malformed wrong-suffix nested relative missing-directory short-archive null preserve-invalid unicode)
endif()
if(TARGET Umicom::kanban_gtk4)
    umicom_public_plan_group(native Umicom::kanban_gtk4 normal no-approval load-only save denied busy retired retained close-load hide-show captured-path discard invalid-file capture-retire null dispatch-budget retire-focus retire-other-focus)
endif()
if(TARGET Umicom::kanban_gtk4)
    umicom_public_plan_group(integration Umicom::kanban_gtk4 normal wrong-board stale-view review-current archived load-only busy retired)
endif()
