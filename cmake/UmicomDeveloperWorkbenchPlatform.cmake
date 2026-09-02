#-----------------------------------------------------------------------------
# Umicom Framework
# File: cmake/UmicomDeveloperWorkbenchPlatform.cmake
#
# PURPOSE:
#   Add the reusable professional developer-workbench runtime to the canonical
#   umicom_developer library after the Framework application platform exists.
#
# ARCHITECTURE:
#   This integration is intentionally additive. It does not replace the existing
#   developer runtime, build graph, test platform, source-control platform or
#   Application Shell. It composes those established capabilities into the
#   command/search/configuration/perspective layer needed by thin IDE products.
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
if(NOT TARGET umicom_developer)
    message(FATAL_ERROR
        "Developer Workbench requires the canonical umicom_developer target")
endif()

# Load the dependency only when the parent build has not already provided its target.
if(NOT TARGET umicom_application)
    message(FATAL_ERROR
        "Developer Workbench requires the Framework application platform")
endif()

target_sources(umicom_developer PRIVATE
    "src/developer_workbench/action_registry.c"
    "src/developer_workbench/builtin_perspectives.c"
    "src/developer_workbench/command_catalogue.c"
    "src/developer_workbench/command_palette.c"
    "src/developer_workbench/command_search_provider.c"
    "src/developer_workbench/command_service.c"
    "src/developer_workbench/commands/ai.c"
    "src/developer_workbench/commands/application.c"
    "src/developer_workbench/commands/build.c"
    "src/developer_workbench/commands/debug.c"
    "src/developer_workbench/commands/edit.c"
    "src/developer_workbench/commands/extensions.c"
    "src/developer_workbench/commands/extensions_settings.c"
    "src/developer_workbench/commands/file.c"
    "src/developer_workbench/commands/help.c"
    "src/developer_workbench/commands/layout.c"
    "src/developer_workbench/commands/navigate.c"
    "src/developer_workbench/commands/project.c"
    "src/developer_workbench/commands/run.c"
    "src/developer_workbench/commands/search.c"
    "src/developer_workbench/commands/settings.c"
    "src/developer_workbench/commands/source_control.c"
    "src/developer_workbench/commands/terminal.c"
    "src/developer_workbench/commands/test.c"
    "src/developer_workbench/commands/tools.c"
    "src/developer_workbench/commands/view.c"
    "src/developer_workbench/commands/window.c"
    "src/developer_workbench/commands/workspace.c"
    "src/developer_workbench/configuration.c"
    "src/developer_workbench/configuration_registry.c"
    "src/developer_workbench/file_search_provider.c"
    "src/developer_workbench/lifecycle.c"
    "src/developer_workbench/operation_history.c"
    "src/developer_workbench/perspective.c"
    "src/developer_workbench/perspective_registry.c"
    "src/developer_workbench/perspectives/ai.c"
    "src/developer_workbench/perspectives/coding.c"
    "src/developer_workbench/perspectives/debugging.c"
    "src/developer_workbench/perspectives/delivery.c"
    "src/developer_workbench/perspectives/design.c"
    "src/developer_workbench/perspectives/source_control.c"
    "src/developer_workbench/perspectives/testing.c"
    "src/developer_workbench/project_wizard.c"
    "src/developer_workbench/quick_open.c"
    "src/developer_workbench/readiness.c"
    "src/developer_workbench/recent_project_provider.c"
    "src/developer_workbench/recent_projects.c"
    "src/developer_workbench/search_engine.c"
    "src/developer_workbench/search_provider.c"
    "src/developer_workbench/search_session.c"
    "src/developer_workbench/search_types.c"
    "src/developer_workbench/start_centre.c"
    "src/developer_workbench/types.c"
    "src/developer_workbench/workbench.c"
)

# Project-wizard planning consumes the Framework application-preset catalogue.
# Runtime command registration and workspace trust are existing Framework
# dependencies; adding them here preserves one implementation instead of
# creating application-specific copies in Studio.
target_link_libraries(umicom_developer PUBLIC
    Umicom::application
    Umicom::runtime
    Umicom::security
)

# Register verification targets only when the developer has enabled testing.
if(BUILD_TESTING)
    # Define the add developer workbench test build helper so parent and application projects
    # apply one consistent rule.
    function(umicom_add_developer_workbench_test target test_name source)
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

    umicom_add_developer_workbench_test(
        umicom-developer-workbench-builtin-perspectives-test
        framework.developer_workbench.builtin-perspectives
        tests/developer_workbench/test_builtin_perspectives.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-command-catalogue-test
        framework.developer_workbench.command-catalogue
        tests/developer_workbench/test_command_catalogue.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-command-service-test
        framework.developer_workbench.command-service
        tests/developer_workbench/test_command_service.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-ai-test
        framework.developer_workbench.commands-ai
        tests/developer_workbench/test_commands_ai.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-application-test
        framework.developer_workbench.commands-application
        tests/developer_workbench/test_commands_application.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-build-test
        framework.developer_workbench.commands-build
        tests/developer_workbench/test_commands_build.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-debug-test
        framework.developer_workbench.commands-debug
        tests/developer_workbench/test_commands_debug.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-edit-test
        framework.developer_workbench.commands-edit
        tests/developer_workbench/test_commands_edit.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-extensions-test
        framework.developer_workbench.commands-extensions
        tests/developer_workbench/test_commands_extensions.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-extensions-settings-test
        framework.developer_workbench.commands-extensions-settings
        tests/developer_workbench/test_commands_extensions_settings.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-file-test
        framework.developer_workbench.commands-file
        tests/developer_workbench/test_commands_file.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-help-test
        framework.developer_workbench.commands-help
        tests/developer_workbench/test_commands_help.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-layout-test
        framework.developer_workbench.commands-layout
        tests/developer_workbench/test_commands_layout.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-navigate-test
        framework.developer_workbench.commands-navigate
        tests/developer_workbench/test_commands_navigate.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-project-test
        framework.developer_workbench.commands-project
        tests/developer_workbench/test_commands_project.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-run-test
        framework.developer_workbench.commands-run
        tests/developer_workbench/test_commands_run.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-search-test
        framework.developer_workbench.commands-search
        tests/developer_workbench/test_commands_search.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-settings-test
        framework.developer_workbench.commands-settings
        tests/developer_workbench/test_commands_settings.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-source-control-test
        framework.developer_workbench.commands-source-control
        tests/developer_workbench/test_commands_source_control.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-terminal-test
        framework.developer_workbench.commands-terminal
        tests/developer_workbench/test_commands_terminal.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-test-test
        framework.developer_workbench.commands-test
        tests/developer_workbench/test_commands_test.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-tools-test
        framework.developer_workbench.commands-tools
        tests/developer_workbench/test_commands_tools.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-view-test
        framework.developer_workbench.commands-view
        tests/developer_workbench/test_commands_view.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-window-test
        framework.developer_workbench.commands-window
        tests/developer_workbench/test_commands_window.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-commands-workspace-test
        framework.developer_workbench.commands-workspace
        tests/developer_workbench/test_commands_workspace.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-configuration-test
        framework.developer_workbench.configuration
        tests/developer_workbench/test_configuration.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-configuration-registry-test
        framework.developer_workbench.configuration-registry
        tests/developer_workbench/test_configuration_registry.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-lifecycle-test
        framework.developer_workbench.lifecycle
        tests/developer_workbench/test_lifecycle.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-operation-history-test
        framework.developer_workbench.operation-history
        tests/developer_workbench/test_operation_history.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-ai-test
        framework.developer_workbench.perspective-ai
        tests/developer_workbench/test_perspective_ai.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-coding-test
        framework.developer_workbench.perspective-coding
        tests/developer_workbench/test_perspective_coding.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-debugging-test
        framework.developer_workbench.perspective-debugging
        tests/developer_workbench/test_perspective_debugging.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-delivery-test
        framework.developer_workbench.perspective-delivery
        tests/developer_workbench/test_perspective_delivery.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-design-test
        framework.developer_workbench.perspective-design
        tests/developer_workbench/test_perspective_design.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-registry-test
        framework.developer_workbench.perspective-registry
        tests/developer_workbench/test_perspective_registry.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-source-control-test
        framework.developer_workbench.perspective-source-control
        tests/developer_workbench/test_perspective_source_control.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-perspective-testing-test
        framework.developer_workbench.perspective-testing
        tests/developer_workbench/test_perspective_testing.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-project-wizard-test
        framework.developer_workbench.project-wizard
        tests/developer_workbench/test_project_wizard.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-readiness-test
        framework.developer_workbench.readiness
        tests/developer_workbench/test_readiness.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-recent-projects-test
        framework.developer_workbench.recent-projects
        tests/developer_workbench/test_recent_projects.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-search-engine-test
        framework.developer_workbench.search-engine
        tests/developer_workbench/test_search_engine.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-search-session-test
        framework.developer_workbench.search-session
        tests/developer_workbench/test_search_session.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-search-types-test
        framework.developer_workbench.search-types
        tests/developer_workbench/test_search_types.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-start-centre-test
        framework.developer_workbench.start-centre
        tests/developer_workbench/test_start_centre.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-types-test
        framework.developer_workbench.types
        tests/developer_workbench/test_types.c
    )
    umicom_add_developer_workbench_test(
        umicom-developer-workbench-workbench-test
        framework.developer_workbench.workbench
        tests/developer_workbench/test_workbench.c
    )
endif()

message(STATUS
    "Umicom professional Developer Workbench runtime enabled")
