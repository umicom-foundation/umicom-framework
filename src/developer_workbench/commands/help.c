/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/help.c
 *
 * PURPOSE:
 *   Define Framework-owned Help commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/help.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "help.welcome",
        "Welcome",
        "Help",
        "Open the Start Centre and welcome content.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "help.documentation",
        "Documentation",
        "Help",
        "Open Umicom documentation.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "help.ai-teacher",
        "AI Teacher",
        "Help",
        "Open the reusable AI Teacher workbench.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "help.shortcuts",
        "Keyboard Shortcuts",
        "Help",
        "Open keyboard shortcut reference/settings.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "help.about",
        "About Umicom",
        "Help",
        "Show application and Framework information.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

/*
 * Provide the developer workbench help commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_help_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench help command without
 * changing their state.
 */
size_t umi_developer_workbench_help_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
