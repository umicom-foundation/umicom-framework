/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/edit.c
 *
 * PURPOSE:
 *   Define Framework-owned Edit commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/edit.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "edit.undo",
        "Undo",
        "Edit",
        "Undo the latest reversible edit.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "edit.redo",
        "Redo",
        "Edit",
        "Redo the latest reverted edit.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "edit.cut",
        "Cut",
        "Edit",
        "Cut the active selection.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "edit.copy",
        "Copy",
        "Edit",
        "Copy the active selection.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "edit.paste",
        "Paste",
        "Edit",
        "Paste into the active editor.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "search.find",
        "Find",
        "Edit",
        "Find text in the active document.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "search.replace",
        "Replace",
        "Edit",
        "Replace text in the active document.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "search.workspace",
        "Find in Files",
        "Edit",
        "Search files in the active workspace.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_edit_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_edit_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
