/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/navigate.c
 *
 * PURPOSE:
 *   Define Framework-owned Navigate commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/navigate.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "navigate.back",
        "Back",
        "Navigate",
        "Navigate to the previous location.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "navigate.forward",
        "Forward",
        "Navigate",
        "Navigate to the next location.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "navigate.file",
        "Go to File",
        "Navigate",
        "Find and open a workspace file.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "navigate.symbol",
        "Go to Symbol",
        "Navigate",
        "Find a symbol in the active workspace.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "navigate.definition",
        "Go to Definition",
        "Navigate",
        "Navigate to the selected symbol definition.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "navigate.references",
        "Find References",
        "Navigate",
        "Find references to the selected symbol.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "navigate.problem.next",
        "Next Problem",
        "Navigate",
        "Move to the next diagnostic.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "navigate.problem.previous",
        "Previous Problem",
        "Navigate",
        "Move to the previous diagnostic.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_navigate_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_navigate_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
