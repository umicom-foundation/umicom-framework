/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/window.c
 *
 * PURPOSE:
 *   Define Framework-owned Window commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/window.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "window.new",
        "New Window",
        "Window",
        "Create another workbench window.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "window.split-right",
        "Split Right",
        "Window",
        "Split the active editor group to the right.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "window.split-down",
        "Split Down",
        "Window",
        "Split the active editor group below.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "window.float",
        "Float Panel",
        "Window",
        "Detach the active panel into a floating window.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

/*
 * Provide the developer workbench window commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_window_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench window command without
 * changing their state.
 */
size_t umi_developer_workbench_window_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
