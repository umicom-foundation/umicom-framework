/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/terminal.c
 *
 * PURPOSE:
 *   Define Framework-owned Terminal commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/terminal.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "terminal.new",
        "New Terminal",
        "Terminal",
        "Create a supervised terminal session.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "terminal.split",
        "Split Terminal",
        "Terminal",
        "Split the active terminal.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "terminal.kill",
        "Kill Terminal",
        "Terminal",
        "Terminate the active terminal session.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_terminal_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_terminal_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
