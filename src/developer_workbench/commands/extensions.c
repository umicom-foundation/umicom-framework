/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/extensions.c
 *
 * PURPOSE:
 *   Define Framework-owned Extensions commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_extensions_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_extensions_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
