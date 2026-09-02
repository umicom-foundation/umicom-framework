/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/extensions_settings.c
 *
 * PURPOSE:
 *   Define Framework-owned Extensions Settings commands. Native lifecycle
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

/*
 * Provide the developer workbench extensions settings commands operation used by this
 * module and its client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_extensions_settings_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench extensions settings
 * command without changing their state.
 */
size_t umi_developer_workbench_extensions_settings_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
