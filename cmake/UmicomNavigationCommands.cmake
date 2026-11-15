# Umicom Framework
# File: cmake/UmicomNavigationCommands.cmake
#
# PURPOSE:
#   Register native source navigation command admission regressions.
#
# AUTHOR AND ORGANISATION:
# Sammy Hegab
# Umicom Foundation
#
# LICENCE:
# MIT
include_guard(GLOBAL)
function(umicom_attach_navigation_command_checks)
    if(NOT TARGET umicom_ui_gtk4 OR NOT BUILD_TESTING OR TARGET umicom-navigation-commands-gtk4-test)
        return()
    endif()
    add_executable(umicom-navigation-commands-gtk4-test
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tests/document/test_navigation_commands_gtk4.c")
    target_link_libraries(umicom-navigation-commands-gtk4-test PRIVATE Umicom::ui_gtk4)
    umicom_apply_warnings(umicom-navigation-commands-gtk4-test)
    umicom_apply_sanitizers(umicom-navigation-commands-gtk4-test)
    if(COMMAND umicom_register_validation_target)
        umicom_register_validation_target(umicom-navigation-commands-gtk4-test)
    endif()
    foreach(command symbol definition references)
        foreach(case open read-only empty multiline busy unbind oversize unknown no-document)
            add_test(NAME framework.document.navigation_commands.gtk4.${command}.${case}
                COMMAND umicom-navigation-commands-gtk4-test navigate.${command} ${case})
            set_tests_properties(framework.document.navigation_commands.gtk4.${command}.${case}
                PROPERTIES TIMEOUT 45 SKIP_RETURN_CODE 77
                LABELS "framework;document;navigation;gtk4;ownership;regression")
        endforeach()
    endforeach()
endfunction()
umicom_attach_navigation_command_checks()
if(NOT TARGET umicom_ui_gtk4)
    cmake_language(DEFER CALL umicom_attach_navigation_command_checks)
endif()
