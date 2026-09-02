/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/tools.c
 *
 * PURPOSE:
 *   Define Framework-owned Tools commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/tools.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "toolchains.open",
        "Toolchains",
        "Tools",
        "Inspect discovered compilers, SDKs and toolchain profiles.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "codeguard.scan",
        "CodeGuard",
        "Tools",
        "Run Framework CodeGuard against the active workspace.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND
    },
    {
        "designer.open",
        "Visual Designer",
        "Tools",
        "Open the Framework visual application designer.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    }
};

/*
 * Provide the developer workbench tools commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_tools_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench tools command without
 * changing their state.
 */
size_t umi_developer_workbench_tools_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
