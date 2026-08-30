/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/source_control.c
 *
 * PURPOSE:
 *   Define Framework-owned Source Control commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/source_control.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "source-control.refresh",
        "Refresh Status",
        "Source Control",
        "Refresh repository status.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "source-control.stage-all",
        "Stage All",
        "Source Control",
        "Stage all selected workspace changes.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "source-control.commit",
        "Commit",
        "Source Control",
        "Create a source-control commit.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "source-control.pull",
        "Pull",
        "Source Control",
        "Pull remote changes.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "source-control.push",
        "Push",
        "Source Control",
        "Push local commits.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "source-control.branches",
        "Branches",
        "Source Control",
        "Open branch management.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "source-control.diff",
        "Compare Changes",
        "Source Control",
        "Open a reusable diff session.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_source_control_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_source_control_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
