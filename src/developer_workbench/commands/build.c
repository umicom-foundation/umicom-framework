/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/build.c
 *
 * PURPOSE:
 *   Define Framework-owned Build commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/build.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "build.configure",
        "Configure",
        "Build",
        "Configure the active project using its Framework toolchain profile.",
        UMI_DEVELOPER_WORKBENCH_ACTION_CONFIGURE,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.build",
        "Build",
        "Build",
        "Compile the active project.",
        UMI_DEVELOPER_WORKBENCH_ACTION_BUILD,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.rebuild",
        "Rebuild",
        "Build",
        "Clean, configure and compile the active project.",
        UMI_DEVELOPER_WORKBENCH_ACTION_REBUILD,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.clean",
        "Clean",
        "Build",
        "Remove generated build output for the active configuration.",
        UMI_DEVELOPER_WORKBENCH_ACTION_CLEAN,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.cancel",
        "Cancel Build",
        "Build",
        "Request cancellation of the active Framework lifecycle operation.",
        UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.install",
        "Install",
        "Build",
        "Install the active configuration into its staging/install prefix.",
        UMI_DEVELOPER_WORKBENCH_ACTION_INSTALL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.package",
        "Package",
        "Build",
        "Generate the configured package through the Framework lifecycle.",
        UMI_DEVELOPER_WORKBENCH_ACTION_PACKAGE,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "build.deploy",
        "Deploy",
        "Build",
        "Deploy a verified package to a selected deployment target.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED | UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_build_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_build_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
