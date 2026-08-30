/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_ui_views.c
 *
 * PURPOSE:
 *   Implement the test terminal ui views behavior for
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
 * Umicom Framework terminal/process/task presentation tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "terminal_fixture.h"

static void assert_kind(UmiUiViewModel *view, const char *expected)
{
    UmiUiValue value;
    assert(view != NULL);
    assert(umi_ui_view_model_get_property(view, "umicom.view-kind", &value) ==
           UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_STRING);
    assert(strcmp(value.string_value, expected) == 0);
}

int main(void)
{
    UmiTerminalTestFixture fixture;
    UmiUiViewModel *terminal = NULL;
    UmiUiViewModel *processes = NULL;
    UmiUiViewModel *tasks = NULL;
    UmiUiViewModel *history = NULL;
    UmiUiValue value;

    assert(terminal_fixture_create(&fixture) == UMI_STATUS_OK);
    assert(umi_terminal_ui_terminal_view_create(
        "view.terminal", fixture.controller, &terminal) == UMI_STATUS_OK);
    assert(umi_terminal_ui_process_view_create(
        "view.processes", fixture.controller, &processes) == UMI_STATUS_OK);
    assert(umi_terminal_ui_task_view_create(
        "view.tasks", fixture.controller, &tasks) == UMI_STATUS_OK);
    assert(umi_terminal_ui_history_view_create(
        "view.history", fixture.controller, &history) == UMI_STATUS_OK);
    assert_kind(terminal, "terminal");
    assert_kind(processes, "processes");
    assert_kind(tasks, "tasks");
    assert_kind(history, "terminal-history");
    assert(umi_ui_view_model_get_property(terminal, "terminal.sessions", &value) ==
           UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER && value.integer_value == 1);
    assert(umi_ui_view_model_get_property(tasks, "task.workers", &value) ==
           UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER && value.integer_value == 1);
    assert(umi_ui_view_model_get_property(tasks, "operation.count", &value) ==
           UMI_STATUS_OK);
    assert(value.kind == UMI_UI_VALUE_INTEGER && value.integer_value == 2);
    umi_ui_view_model_destroy(history);
    umi_ui_view_model_destroy(tasks);
    umi_ui_view_model_destroy(processes);
    umi_ui_view_model_destroy(terminal);
    terminal_fixture_destroy(&fixture);
    return 0;
}
