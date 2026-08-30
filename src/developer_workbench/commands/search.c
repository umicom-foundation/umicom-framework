/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/search.c
 *
 * PURPOSE:
 *   Define Framework-owned Search commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/search.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "search.everywhere",
        "Search Everywhere",
        "Search",
        "Search commands, files and registered workbench providers.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "search.refresh",
        "Refresh Search",
        "Search",
        "Refresh workspace search indexes.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND
    },
    {
        "output.clear",
        "Clear Output",
        "Search",
        "Clear selected output channel.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    },
    {
        "output.follow",
        "Follow Output",
        "Search",
        "Toggle automatic scrolling of live output.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_NONE
    },
    {
        "problems.clear",
        "Clear Problems",
        "Search",
        "Clear resolved/transient Problems entries.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_MUTATES_STATE
    }
};

const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_search_commands(void)
{
    return COMMANDS;
}

size_t umi_developer_workbench_search_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
