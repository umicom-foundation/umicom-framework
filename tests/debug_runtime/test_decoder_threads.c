/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_threads.c
 *
 * PURPOSE:
 *   Verify the bounded threads DAP decoder.
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
#include "umicom/debug_runtime/decoders/threads.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeThreadList result;
    assert(umi_debug_runtime_decode_threads(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"threads\",\"body\":{\"threads\":[{\"id\":7,\"name\":\"main\"}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(result.items[0].id == 7U);
    assert(strcmp(result.items[0].name, "main") == 0);
    return 0;
}
