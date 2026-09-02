/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/project.c
 *
 * PURPOSE:
 *   Define Framework-owned Project commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/project.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "project.preset.select",
        "Application Presets",
        "Project",
        "Choose a Framework application preset.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "project.toolchain.select",
        "Toolchain Selection",
        "Project",
        "Choose the project toolchain profile.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "project.create",
        "Create Project",
        "Project",
        "Create the planned project/repository.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "workspace.refresh",
        "Refresh Workspace",
        "Project",
        "Refresh workspace/project models.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE
    },
    {
        "file.new",
        "New File",
        "Project",
        "Create a new file.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "folder.new",
        "New Folder",
        "Project",
        "Create a new folder.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "repository.clone",
        "Clone Repository",
        "Project",
        "Clone and open a repository.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

/*
 * Provide the developer workbench project commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_project_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench project command without
 * changing their state.
 */
size_t umi_developer_workbench_project_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
