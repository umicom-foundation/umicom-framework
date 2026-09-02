/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/extensions.c
 *
 * PURPOSE:
 *   Define Framework-owned Extensions commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/extensions.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "extensions.open",
        "Extensions",
        "Extensions",
        "Open extension management.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "extensions.refresh",
        "Refresh Extensions",
        "Extensions",
        "Refresh installed extension inventory.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "extensions.install",
        "Install Extension",
        "Extensions",
        "Install a verified extension.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

/*
 * Provide the developer workbench extensions commands operation used by this module and
 * its client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_extensions_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench extensions command
 * without changing their state.
 */
size_t umi_developer_workbench_extensions_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
