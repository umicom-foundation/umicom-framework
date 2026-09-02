/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/test_platform/command.c
 *
 * PURPOSE:
 *   Implement the canonical Framework-owned Test Explorer command catalogue.
 *   Studio and future applications consume these identities without defining
 *   alternative command semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/test_platform/command.h"

#include <string.h>

#define TEST_COMMAND(command_kind, command_id, command_label, key, tests, selection, results, active, inactive, mutates) \
    {                                                                                                                   \
        (uint32_t)sizeof(UmiTestPlatformCommandDescriptor),                                                            \
        UMI_TEST_PLATFORM_COMMAND_API_VERSION, command_kind, command_id,                                                \
        command_label, key, tests, selection, results, active, inactive, mutates                                      \
    }

static const UmiTestPlatformCommandDescriptor COMMANDS[] = {
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_EXPLORER,
        "test.explorer.open", "Open Test Explorer", "Ctrl+Shift+T", 0, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_REFRESH,
        "test.explorer.refresh", "Refresh Test Explorer", "", 0, 0, 0, 0, 1, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_DISCOVER,
        "test.discover", "Discover Tests", "", 0, 0, 0, 0, 1, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_RUN_ALL,
        "test.run-all", "Run All Tests", "Ctrl+; A", 1, 0, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_RUN_SELECTED,
        "test.run-selected", "Run Selected Tests", "Ctrl+; R", 1, 1, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_DEBUG_SELECTED,
        "test.debug-selected", "Debug Selected Tests", "Ctrl+; D", 1, 1, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_COVERAGE_ALL,
        "test.coverage-all", "Run All with Coverage", "", 1, 0, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_COVERAGE_SELECTED,
        "test.coverage-selected", "Run Selected with Coverage", "", 1, 1, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_RERUN_FAILED,
        "test.rerun-failed", "Rerun Failed Tests", "Ctrl+; F", 1, 0, 1, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_RERUN_LAST,
        "test.rerun-last", "Rerun Last Test Run", "Ctrl+; L", 1, 0, 1, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_RERUN_FLAKY,
        "test.rerun-flaky", "Rerun Flaky Tests", "", 1, 0, 1, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_REPEAT_SELECTED,
        "test.repeat-selected", "Repeat Selected Tests", "", 1, 1, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_STOP,
        "test.stop", "Stop Test Run", "Shift+F5", 0, 0, 0, 1, 0, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_SELECT_PROFILE,
        "test.profile.select", "Select Test Profile", "", 0, 0, 0, 0, 1, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_FILTER,
        "test.filter.focus", "Focus Test Filter", "Ctrl+F", 0, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_FILTER_FAILED,
        "test.filter.failed", "Show Failed Tests", "", 0, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_FILTER_NOT_RUN,
        "test.filter.not-run", "Show Tests Not Run", "", 0, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_CLEAR_FILTER,
        "test.filter.clear", "Clear Test Filter", "Escape", 0, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_GROUP_HIERARCHY,
        "test.group.hierarchy", "Group Tests by Hierarchy", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_GROUP_SUITE,
        "test.group.suite", "Group Tests by Suite", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_GROUP_FRAMEWORK,
        "test.group.framework", "Group Tests by Framework", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_GROUP_OUTCOME,
        "test.group.outcome", "Group Tests by Outcome", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_SORT_NAME,
        "test.sort.name", "Sort Tests by Name", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_SORT_DURATION,
        "test.sort.duration", "Sort Tests by Duration", "", 1, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_SORT_OUTCOME,
        "test.sort.outcome", "Sort Tests by Outcome", "", 1, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_EXPAND_ALL,
        "test.explorer.expand-all", "Expand All Tests", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_COLLAPSE_ALL,
        "test.explorer.collapse-all", "Collapse All Tests", "", 1, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_NEXT_FAILURE,
        "test.failure.next", "Go to Next Test Failure", "F8", 0, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_PREVIOUS_FAILURE,
        "test.failure.previous", "Go to Previous Test Failure", "Shift+F8", 0, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_SOURCE,
        "test.source.open", "Open Test Source", "Enter", 1, 1, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_OUTPUT,
        "test.output.open", "Open Test Output", "", 0, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_COVERAGE,
        "test.coverage.open", "Open Coverage", "", 0, 0, 0, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_BENCHMARKS,
        "test.benchmarks.open", "Open Benchmarks", "", 0, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_HISTORY,
        "test.history.open", "Open Test History", "", 0, 0, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_OPEN_RERUN_PLANNER,
        "test.rerun-planner.open", "Open Rerun Planner", "", 1, 0, 1, 0, 1, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_CLEAR_RESULTS,
        "test.results.clear", "Clear Test Results", "", 0, 0, 1, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_CLEAR_OUTPUT,
        "test.output.clear", "Clear Test Output", "", 0, 0, 1, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_CLEAR_COVERAGE,
        "test.coverage.clear", "Clear Coverage", "", 0, 0, 0, 0, 1, 1),
    TEST_COMMAND(UMI_TEST_COMMAND_COPY_RESULT,
        "test.result.copy", "Copy Test Result", "Ctrl+C", 0, 1, 1, 0, 0, 0),
    TEST_COMMAND(UMI_TEST_COMMAND_EXPORT_REPORT,
        "test.report.export", "Export Test Report", "", 0, 0, 1, 0, 1, 0)
};

#undef TEST_COMMAND

/*
 * Return the number of records represented by test platform command without changing their
 * state.
 */
size_t umi_test_platform_command_count(void)
{
    return sizeof(COMMANDS) / sizeof(COMMANDS[0]);
}

/*
 * Find test platform command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiTestPlatformCommandDescriptor *umi_test_platform_command_at(
    size_t position)
{
    return position < umi_test_platform_command_count() ? &COMMANDS[position]
                                                         : NULL;
}

/*
 * Find test platform command while leaving the underlying catalogue or model owned by this
 * module.
 */
const UmiTestPlatformCommandDescriptor *umi_test_platform_command_find(
    const char *command_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (command_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_test_platform_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(COMMANDS[index].id, command_id) == 0) return &COMMANDS[index];
    }
    return NULL;
}

/*
 * Provide the test platform command for kind operation used by this module and its client
 * applications.
 */
const UmiTestPlatformCommandDescriptor *umi_test_platform_command_for_kind(
    UmiTestPlatformCommandKind kind)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < umi_test_platform_command_count(); ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (COMMANDS[index].kind == kind) return &COMMANDS[index];
    }
    return NULL;
}
