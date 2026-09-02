/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/commands/test.c
 *
 * PURPOSE:
 *   Define Framework-owned Test commands. Native lifecycle
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
#include "umicom/developer_workbench/commands/test.h"

static const UmiDeveloperWorkbenchCommandDefinition COMMANDS[] = {
    {
        "test.discover",
        "Discover Tests",
        "Test",
        "Refresh Framework test discovery.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND
    },
    {
        "test.run-all",
        "Run All Tests",
        "Test",
        "Execute tests through the active Framework lifecycle configuration.",
        UMI_DEVELOPER_WORKBENCH_ACTION_TEST,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "test.run-selected",
        "Run Selected",
        "Test",
        "Execute selected Test Explorer items.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND | UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    },
    {
        "test.debug-selected",
        "Debug Selected",
        "Test",
        "Debug selected tests.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND
    },
    {
        "test.rerun-failed",
        "Rerun Failed",
        "Test",
        "Rerun failed tests.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND
    },
    {
        "test.coverage-all",
        "Run with Coverage",
        "Test",
        "Run all tests and capture coverage.",
        UMI_DEVELOPER_WORKBENCH_ACTION_EXTERNAL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_WORKSPACE | UMI_DEVELOPER_WORKBENCH_COMMAND_REQUIRES_TRUST | UMI_DEVELOPER_WORKBENCH_COMMAND_BACKGROUND
    },
    {
        "test.stop",
        "Stop Tests",
        "Test",
        "Request cancellation of the active test operation.",
        UMI_DEVELOPER_WORKBENCH_ACTION_CANCEL,
        UMI_DEVELOPER_WORKBENCH_COMMAND_AUDITED
    }
};

/*
 * Provide the developer workbench test commands operation used by this module and its
 * client applications.
 */
const UmiDeveloperWorkbenchCommandDefinition *
umi_developer_workbench_test_commands(void)
{
    return COMMANDS;
}

/*
 * Return the number of records represented by developer workbench test command without
 * changing their state.
 */
size_t umi_developer_workbench_test_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}
