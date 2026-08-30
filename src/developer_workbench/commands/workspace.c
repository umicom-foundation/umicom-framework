/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/workspace.c
 *
 * PURPOSE:
 *   Define Framework-owned Workspace commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/workspace.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "workspace.trust",
        "Trust Workspace",
        "Workspace",
        "Mark the active workspace as trusted.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "workspace.restrict",
        "Restrict Workspace",
        "Workspace",
        "Run the active workspace with restricted command permissions.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "workspace.close",
        "Close Workspace",
        "Workspace",
        "Close the active workspace.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_workspace_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_workspace_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
