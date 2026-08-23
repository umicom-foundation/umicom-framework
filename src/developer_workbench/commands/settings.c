/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/settings.c
 *
 * PURPOSE:
 *   Define Framework-owned Settings commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
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

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_settings_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_settings_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
