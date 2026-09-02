/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_diagnostic_event.c
 *
 * PURPOSE:
 *   Implement the test diagnostic event behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/diagnostics/event.h"
/*
 * Exercise on event and return a clear result when the behaviour no longer matches its
 * contract.
 */
static void on_event(const UmiDiagnosticEvent *event, void *data)
{
    size_t *count = (size_t *)data;
    assert(event != NULL);
    ++(*count);
}
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDiagnosticEventStream *stream = NULL;
    UmiDiagnosticEvent event = {0};
    UmiDiagnosticEvent copy;
    size_t received = 0U;
    assert(umi_diagnostic_event_stream_create(&stream) == UMI_STATUS_OK);
    umi_diagnostic_event_stream_set_sink(stream, on_event, &received);
    event.kind = UMI_DIAGNOSTIC_EVENT_DIAGNOSTIC_CREATED;
    (void)strcpy(event.diagnostic_id, "d1");
    assert(umi_diagnostic_event_stream_append(stream, &event) == UMI_STATUS_OK);
    assert(umi_diagnostic_event_stream_at(stream, 0U, &copy) == UMI_STATUS_OK);
    assert(received == 1U && copy.sequence == 1U);
    umi_diagnostic_event_stream_destroy(stream);
    return 0;
}
