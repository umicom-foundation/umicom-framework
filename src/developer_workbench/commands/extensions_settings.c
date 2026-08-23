/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/extensions_settings.c
 *
 * PURPOSE:
 *   Define Framework-owned Extensions Settings commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/extensions_settings.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "extensions.settings",
        "Extension Settings",
        "Extensions",
        "Open extension-specific settings.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "settings.reset",
        "Reset Settings",
        "Settings",
        "Reset selected settings to defaults.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_extensions_settings_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_extensions_settings_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
