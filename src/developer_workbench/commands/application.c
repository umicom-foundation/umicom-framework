/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/application.c
 *
 * PURPOSE:
 *   Define Framework-owned Application commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/application.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "application.launch.studio",
        "Launch Studio",
        "Application",
        "Launch or focus Umicom Studio.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "application.launch.desk",
        "Launch Desk",
        "Application",
        "Launch or focus Umicom Desk.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "application.launch.trader",
        "Launch Trader",
        "Application",
        "Launch or focus Umicom Trader.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_application_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_application_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
