#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomApplicationShellPlatform.cmake
#
# PURPOSE:
#   Attach the complete reusable Application Shell platform to the canonical
#   Umicom::application library. This keeps menus, toolbars, views, panels,
#   command routing, state, drag/drop, personas and IDE/workstation shell
#   profiles in Framework.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
include_guard(GLOBAL)

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "Application Shell requires the canonical umicom_application target")
endif()

target_sources(umicom_application PRIVATE
    "src/application_shell/types.c"
    "src/application_shell/registry.c"
    "src/application_shell/profile.c"
    "src/application_shell/command_bridge.c"
    "src/application_shell/state.c"
    "src/application_shell/drag_drop.c"
    "src/application_shell/layout.c"
    "src/application_shell/builtin_profiles.c"
    "src/application_shell/persona.c"
    "src/application_shell/profiles/file_menu.c"
    "src/application_shell/profiles/edit_menu.c"
    "src/application_shell/profiles/view_menu.c"
    "src/application_shell/profiles/navigate_menu.c"
    "src/application_shell/profiles/build_menu.c"
    "src/application_shell/profiles/run_menu.c"
    "src/application_shell/profiles/test_menu.c"
    "src/application_shell/profiles/source_control_menu.c"
    "src/application_shell/profiles/tools_menu.c"
    "src/application_shell/profiles/window_menu.c"
    "src/application_shell/profiles/help_menu.c"
    "src/application_shell/profiles/main_toolbar.c"
    "src/application_shell/profiles/activity_bar.c"
    "src/application_shell/profiles/status_bar.c"
    "src/application_shell/profiles/start_center.c"
    "src/application_shell/profiles/project_explorer.c"
    "src/application_shell/profiles/editor.c"
    "src/application_shell/profiles/search.c"
    "src/application_shell/profiles/problems.c"
    "src/application_shell/profiles/output.c"
    "src/application_shell/profiles/terminal.c"
    "src/application_shell/profiles/source_control.c"
    "src/application_shell/profiles/test_explorer.c"
    "src/application_shell/profiles/debug.c"
    "src/application_shell/profiles/build_dashboard.c"
    "src/application_shell/profiles/deploy_dashboard.c"
    "src/application_shell/profiles/extensions.c"
    "src/application_shell/profiles/settings.c"
    "src/application_shell/profiles/ai_assistant.c"
    "src/application_shell/profiles/project_wizard.c"
    "src/application_shell/profiles/layout_manager.c"
    "src/application_shell/profiles/application_launcher.c"
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add application shell test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_application_shell_test target test_name source)
        # Configure the optional target only when its feature has created it.
        if(TARGET "${target}")
            return()
        endif()

        add_executable(
            "${target}"
            "${CMAKE_CURRENT_LIST_DIR}/../${source}"
        )
        target_link_libraries("${target}" PRIVATE Umicom::Framework)

        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_warnings)
            umicom_apply_warnings("${target}")
        endif()

        # Use the shared build helper when it is available from the parent composition.
        if(COMMAND umicom_apply_sanitizers)
            umicom_apply_sanitizers("${target}")
        endif()

        add_test(NAME "${test_name}" COMMAND "${target}")
    endfunction()

    umicom_add_application_shell_test(
        umicom-application-shell-registry-test
        framework.application_shell.registry
        tests/application_shell/test_registry.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-command-bridge-test
        framework.application_shell.command_bridge
        tests/application_shell/test_command_bridge.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-state-test
        framework.application_shell.state
        tests/application_shell/test_state.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-drag-drop-test
        framework.application_shell.drag_drop
        tests/application_shell/test_drag_drop.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-layout-test
        framework.application_shell.layout
        tests/application_shell/test_layout.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-builtins-test
        framework.application_shell.builtin_profiles
        tests/application_shell/test_builtin_profiles.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-persona-test
        framework.application_shell.persona
        tests/application_shell/test_persona.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-file-menu-test
        framework.application_shell.profile.file-menu
        tests/application_shell/test_profile_file_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-edit-menu-test
        framework.application_shell.profile.edit-menu
        tests/application_shell/test_profile_edit_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-view-menu-test
        framework.application_shell.profile.view-menu
        tests/application_shell/test_profile_view_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-navigate-menu-test
        framework.application_shell.profile.navigate-menu
        tests/application_shell/test_profile_navigate_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-build-menu-test
        framework.application_shell.profile.build-menu
        tests/application_shell/test_profile_build_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-run-menu-test
        framework.application_shell.profile.run-menu
        tests/application_shell/test_profile_run_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-test-menu-test
        framework.application_shell.profile.test-menu
        tests/application_shell/test_profile_test_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-source-control-menu-test
        framework.application_shell.profile.source-control-menu
        tests/application_shell/test_profile_source_control_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-tools-menu-test
        framework.application_shell.profile.tools-menu
        tests/application_shell/test_profile_tools_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-window-menu-test
        framework.application_shell.profile.window-menu
        tests/application_shell/test_profile_window_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-help-menu-test
        framework.application_shell.profile.help-menu
        tests/application_shell/test_profile_help_menu.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-main-toolbar-test
        framework.application_shell.profile.main-toolbar
        tests/application_shell/test_profile_main_toolbar.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-activity-bar-test
        framework.application_shell.profile.activity-bar
        tests/application_shell/test_profile_activity_bar.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-status-bar-test
        framework.application_shell.profile.status-bar
        tests/application_shell/test_profile_status_bar.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-start-center-test
        framework.application_shell.profile.start-center
        tests/application_shell/test_profile_start_center.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-project-explorer-test
        framework.application_shell.profile.project-explorer
        tests/application_shell/test_profile_project_explorer.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-editor-test
        framework.application_shell.profile.editor
        tests/application_shell/test_profile_editor.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-search-test
        framework.application_shell.profile.search
        tests/application_shell/test_profile_search.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-problems-test
        framework.application_shell.profile.problems
        tests/application_shell/test_profile_problems.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-output-test
        framework.application_shell.profile.output
        tests/application_shell/test_profile_output.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-terminal-test
        framework.application_shell.profile.terminal
        tests/application_shell/test_profile_terminal.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-source-control-test
        framework.application_shell.profile.source-control
        tests/application_shell/test_profile_source_control.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-test-explorer-test
        framework.application_shell.profile.test-explorer
        tests/application_shell/test_profile_test_explorer.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-debug-test
        framework.application_shell.profile.debug
        tests/application_shell/test_profile_debug.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-build-dashboard-test
        framework.application_shell.profile.build-dashboard
        tests/application_shell/test_profile_build_dashboard.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-deploy-dashboard-test
        framework.application_shell.profile.deploy-dashboard
        tests/application_shell/test_profile_deploy_dashboard.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-extensions-test
        framework.application_shell.profile.extensions
        tests/application_shell/test_profile_extensions.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-settings-test
        framework.application_shell.profile.settings
        tests/application_shell/test_profile_settings.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-ai-assistant-test
        framework.application_shell.profile.ai-assistant
        tests/application_shell/test_profile_ai_assistant.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-project-wizard-test
        framework.application_shell.profile.project-wizard
        tests/application_shell/test_profile_project_wizard.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-layout-manager-test
        framework.application_shell.profile.layout-manager
        tests/application_shell/test_profile_layout_manager.c
    )
    umicom_add_application_shell_test(
        umicom-application-shell-profile-application-launcher-test
        framework.application_shell.profile.application-launcher
        tests/application_shell/test_profile_application_launcher.c
    )
endif()

message(STATUS
    "Umicom reusable Application Shell platform enabled")
