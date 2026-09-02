/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/settings.c
 *
 * PURPOSE:
 *   Define Framework-owned Settings commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/settings.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "settings.open",
        "Settings",
        "Settings",
        "Open application and Framework settings.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "settings.search",
        "Search Settings",
        "Settings",
        "Search settings by name/category.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "settings.keybindings",
        "Keyboard Shortcuts",
        "Settings",
        "Edit keyboard shortcuts.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

/*
 * Provide the developer workbench settings commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_settings_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench settings command without
 * changing their state.
 */
size_t umi_developer_workbench_settings_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
