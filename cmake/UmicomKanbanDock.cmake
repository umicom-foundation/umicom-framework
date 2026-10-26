# Umicom Framework
# File: cmake/UmicomKanbanDock.cmake
# PURPOSE: Qualify native document ownership and the Studio planning catalogue.
# AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
# LICENCE: MIT
if(NOT TARGET umicom_developer)
    return()
endif()
include(GNUInstallDirs)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/studio_runtime/surface_profiles/kanban.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/studio_runtime/surface_profiles" COMPONENT Framework)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/studio_runtime/activation_profiles/kanban.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/studio_runtime/activation_profiles" COMPONENT Framework)
install(FILES
    "${CMAKE_CURRENT_LIST_DIR}/../include/umicom/studio_runtime/command_aliases/view_kanban.h"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/umicom/studio_runtime/command_aliases" COMPONENT Framework)
if(COMMAND umicom_release_inventory_own_headers)
    umicom_release_inventory_own_headers(umicom_developer
        include/umicom/studio_runtime/surface_profiles/kanban.h
        include/umicom/studio_runtime/activation_profiles/kanban.h
        include/umicom/studio_runtime/command_aliases/view_kanban.h)
endif()
if(NOT BUILD_TESTING)
    return()
endif()
function(umicom_kanban_dock_group group owner)
    set(target "umi-kanban-dock-${group}-test")
    if(TARGET ${target})
        return()
    endif()
    if(group STREQUAL "focus_handoff")
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_gtk4/test_focus_handoff.c")
    else()
        add_executable(${target} "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_dock/test_${group}.c")
    endif()
    target_link_libraries(${target} PRIVATE ${owner})
    set_target_properties(${target} PROPERTIES C_STANDARD 23 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(${target})
    endif()
    if(COMMAND umicom_target_qualification_inputs)
        umicom_target_qualification_inputs(${target} FILES
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/kanban_dock/fixture.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/support/main_context.h"
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../adapters/gtk4/focus_handoff_private.h")
    endif()
    foreach(case IN LISTS ARGN)
        set(test "framework.kanban.dock.${group}.${case}")
        set(directory "${CMAKE_CURRENT_BINARY_DIR}/qualification/kanban-dock/${group}/${case}")
        file(MAKE_DIRECTORY "${directory}")
        if(group STREQUAL "document")
            add_test(NAME ${test} COMMAND ${target} ${case} "${directory}")
        else()
            add_test(NAME ${test} COMMAND ${target} ${case})
        endif()
        set_tests_properties(${test} PROPERTIES TIMEOUT 60
            WORKING_DIRECTORY "${directory}" LABELS "framework;kanban;ownership;regression")
        if(NOT group STREQUAL "catalogue")
            set_tests_properties(${test} PROPERTIES SKIP_RETURN_CODE 77 ENVIRONMENT "G_DEBUG=fatal-warnings")
        endif()
        if(COMMAND umicom_test_impact_owners)
            umicom_test_impact_owners(${test} TARGETS ${target})
        endif()
    endforeach()
endfunction()
umicom_kanban_dock_group(catalogue Umicom::developer identity activation alias contract layout old-layout)
if(TARGET Umicom::kanban_gtk4)
    umicom_kanban_dock_group(focus_handoff Umicom::kanban_gtk4 busy retire reentrant-retire unrelated unrooted)
    umicom_kanban_dock_group(native Umicom::kanban_gtk4 normal null same-window ordinary-window removed-host different-application host-removal-during-dock return-during-dock duplicate return present host-removed board-removed pane-finalized hide-host redock retained-control)
    umicom_kanban_dock_group(document Umicom::kanban_gtk4 busy close-refuses host-preserves return-preserves save-docked save-then-return host-close-while-saving retire-while-saving)
endif()
