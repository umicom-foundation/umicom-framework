/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/debug.c
 *
 * PURPOSE:
 *   Define Framework-owned Debug commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/debug.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "debug.start",
        "Start Debugging",
        "Debug",
        "Launch the active debug configuration.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "debug.attach",
        "Attach to Process",
        "Debug",
        "Attach the debugger to a process.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "debug.breakpoints",
        "Breakpoints",
        "Debug",
        "Open breakpoint management.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "debug.continue",
        "Continue",
        "Debug",
        "Continue the active debug session.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "debug.stop",
        "Stop Debugging",
        "Debug",
        "Stop the active debug session.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_debug_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_debug_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
