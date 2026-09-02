/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_event.c
 *
 * PURPOSE:
 *   Verify the bounded event DAP decoder.
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
#include "umicom/debug_runtime/decoders/event.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeEvent result;
    assert(umi_debug_runtime_decode_event(
        "{\"seq\":9,\"type\":\"event\",\"event\":\"stopped\",\"body\":{"
        "\"reason\":\"breakpoint\",\"threadId\":7,\"allThreadsStopped\":true}}",
        &result) == UMI_STATUS_OK);
    assert(strcmp(result.event, "stopped") == 0);
    assert(result.thread_id == 7U);
    assert(result.all_threads_stopped == 1);
    return 0;
}
