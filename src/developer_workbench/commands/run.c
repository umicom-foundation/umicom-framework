/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/run.c
 *
 * PURPOSE:
 *   Define Framework-owned Run commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/run.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "run.start",
        "Run",
        "Run",
        "Launch the active executable through Framework process supervision.",
        UMI_DEVELOPER_WORKBENCH_ACTION_RUN,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "run.select-configuration",
        "Run With",
        "Run",
        "Choose the active run configuration.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "run.stop",
        "Stop",
        "Run",
        "Request cancellation/termination of the active run operation.",
        UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "run.restart",
        "Restart",
        "Run",
        "Restart the active launch configuration.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

/*
 * Provide the developer workbench run commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_run_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench run command without
 * changing their state.
 */
size_t umi_developer_workbench_run_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
