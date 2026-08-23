/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/file.c
 *
 * PURPOSE:
 *   Define Framework-owned File commands. Native lifecycle
 *   actions are executable directly; product/UI actions are enabled only after
 *   an application binds a handler through the action registry.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/commands/file.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "project.new",
        "New Project",
        "File",
        "Create a new project from a Framework project/application template.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "file.open",
        "Open File",
        "File",
        "Open an existing file in the active editor group.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "workspace.open-folder",
        "Open Folder",
        "File",
        "Open or switch the active workspace folder.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "file.save",
        "Save",
        "File",
        "Save the active document.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "file.save-all",
        "Save All",
        "File",
        "Save all modified documents.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "file.close",
        "Close Editor",
        "File",
        "Close the active editor.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "application.exit",
        "Exit",
        "File",
        "Request graceful application shutdown.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_file_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_file_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
