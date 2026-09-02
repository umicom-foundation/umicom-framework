/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_terminal_event.c
 *
 * PURPOSE:
 *   Implement the test terminal event behavior for
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
 * Umicom Framework terminal event-journal tests.
 * Created by Sammy Hegab, Umicom Foundation. Licence: MIT.
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/terminal/event.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiTerminalEventLog *log = NULL;
    UmiTerminalEvent event = {0};
    UmiTerminalEvent actual;
    assert(umi_terminal_event_log_create(2U, &log) == UMI_STATUS_OK);
    event.kind = UMI_TERMINAL_EVENT_SESSION_OPENED;
    (void)snprintf(event.subject_id, sizeof(event.subject_id), "%s", "one");
    (void)snprintf(event.message, sizeof(event.message), "%s", "opened");
    assert(umi_terminal_event_log_append(log, &event) == UMI_STATUS_OK);
    event.kind = UMI_TERMINAL_EVENT_HISTORY_CLEARED;
    assert(umi_terminal_event_log_append(log, &event) == UMI_STATUS_OK);
    event.kind = UMI_TERMINAL_EVENT_SESSION_CLOSED;
    assert(umi_terminal_event_log_append(log, &event) == UMI_STATUS_OK);
    assert(umi_terminal_event_log_count(log) == 2U);
    assert(umi_terminal_event_log_at(log, 0U, &actual) == UMI_STATUS_OK);
    assert(actual.kind == UMI_TERMINAL_EVENT_HISTORY_CLEARED);
    assert(strcmp(umi_terminal_event_kind_text(actual.kind), "history-cleared") == 0);
    umi_terminal_event_log_clear(log);
    assert(umi_terminal_event_log_count(log) == 0U);
    umi_terminal_event_log_destroy(log);
    return 0;
}
