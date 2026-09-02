/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_history.c
 *
 * PURPOSE:
 *   Implement the test terminal history behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework terminal-history tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/terminal/history.h"

/*
 * Exercise entry and return a clear result when the behaviour no longer matches its
 * contract.
 */
static UmiTerminalHistoryEntry entry(const char *command, int exit_code)
{
    UmiTerminalHistoryEntry value = {0};
    (void)snprintf(value.session_id, sizeof(value.session_id), "%s", "primary");
    (void)snprintf(value.working_directory, sizeof(value.working_directory), "%s", ".");
    (void)snprintf(value.command, sizeof(value.command), "%s", command);
    value.completed = 1;
    value.exit_code = exit_code;
    return value;
}

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTerminalHistory *history = NULL;
    UmiTerminalHistoryEntry actual;
    UmiTerminalHistoryEntry first = entry("cmake --build build", 0);
    UmiTerminalHistoryEntry second = entry("ctest --test-dir build", 2);
    UmiTerminalHistoryStats stats;
    size_t index = 0U;
    size_t cursor;

    assert(umi_terminal_history_create(2U, &history) == UMI_STATUS_OK);
    assert(umi_terminal_history_append(history, &first) == UMI_STATUS_OK);
    assert(umi_terminal_history_append(history, &second) == UMI_STATUS_OK);
    stats = umi_terminal_history_stats(history);
    assert(stats.count == 2U && stats.succeeded == 1U && stats.failed == 1U);
    assert(umi_terminal_history_search(history, "ctest", 1U, &index, &actual) ==
           UMI_STATUS_OK);
    assert(index == 1U && strcmp(actual.command, second.command) == 0);
    cursor = stats.count;
    assert(umi_terminal_history_previous(history, &cursor, &actual) == UMI_STATUS_OK);
    assert(strcmp(actual.command, second.command) == 0);
    umi_terminal_history_clear(history);
    assert(umi_terminal_history_stats(history).count == 0U);
    umi_terminal_history_destroy(history);
    return 0;
}
