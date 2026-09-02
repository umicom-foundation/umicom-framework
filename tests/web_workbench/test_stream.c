/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_stream.c
 * PURPOSE: Verify SSE ingestion, stream filtering and paused-session behaviour.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/stream.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiWebWorkbenchStreamSession session;
    const UmiWebWorkbenchStreamMessage *matches[4U];
    umi_web_workbench_stream_init(&session, "prices", "Prices",
        UMI_WEB_WORKBENCH_STREAM_SSE, "https://api.example.test/prices");
    assert(umi_web_workbench_stream_set_phase(&session,
        UMI_WEB_WORKBENCH_RUNNING) == UMI_STATUS_OK);
    assert(umi_web_workbench_stream_ingest_sse(&session,
        "event: price\ndata: {\"symbol\":\"EURUSD\"}\n\n", 100U) == UMI_STATUS_OK);
    assert(session.message_count == 1U);
    assert(strcmp(session.messages[0].event_name, "price") == 0);
    assert(umi_web_workbench_stream_query(&session, "EURUSD",
        UMI_WEB_WORKBENCH_MESSAGE_INBOUND, true, matches, 4U) == 1U);
    session.paused = true;
    assert(umi_web_workbench_stream_add(&session, UMI_WEB_WORKBENCH_MESSAGE_INBOUND,
        "price", "x", 1U, 101U) == UMI_STATUS_BUSY);
    return 0;
}
