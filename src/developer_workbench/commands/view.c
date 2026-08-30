/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/view.c
 *
 * PURPOSE:
 *   Define Framework-owned View commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/view.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "workbench.command-palette",
        "Command Palette",
        "View",
        "Search and execute Framework commands.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "workbench.quick-open",
        "Quick Open",
        "View",
        "Open a workspace file by name or path.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "view.explorer",
        "Project Explorer",
        "View",
        "Show or focus the Project Explorer.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.search",
        "Search",
        "View",
        "Show or focus the workspace Search view.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.source-control",
        "Source Control",
        "View",
        "Show or focus Source Control.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.test-explorer",
        "Test Explorer",
        "View",
        "Show or focus Test Explorer.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.terminal",
        "Terminal",
        "View",
        "Show or focus the integrated terminal.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.problems",
        "Problems",
        "View",
        "Show or focus compiler/runtime problems.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.output",
        "Output",
        "View",
        "Show or focus structured output.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.zen",
        "Zen Mode",
        "View",
        "Toggle distraction-free workbench presentation.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "view.build-dashboard",
        "Build Dashboard",
        "View",
        "Show or focus the Build Dashboard.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "view.debug",
        "Run and Debug",
        "View",
        "Show or focus Run and Debug.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_view_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_view_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
