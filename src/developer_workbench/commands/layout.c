/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/layout.c
 *
 * PURPOSE:
 *   Define Framework-owned Layout commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/layout.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "layout.save",
        "Save Layout",
        "Layout",
        "Persist the current semantic workbench layout.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "layout.restore",
        "Restore Layout",
        "Layout",
        "Restore a saved semantic layout.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "layout.manager",
        "Layout Manager",
        "Layout",
        "Open layout browser/manager.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_layout_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_layout_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
