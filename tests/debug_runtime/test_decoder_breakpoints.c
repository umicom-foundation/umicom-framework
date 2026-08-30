/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_breakpoints.c
 *
 * PURPOSE:
 *   Verify the bounded breakpoints DAP decoder.
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
#include "umicom/debug_runtime/decoders/breakpoints.h"

int main(void)
{
    UmiDebugRuntimeBreakpointList result;
    assert(umi_debug_runtime_decode_breakpoints(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"setBreakpoints\",\"body\":{\"breakpoints\":[{"
        "\"id\":1,\"verified\":true,\"line\":10}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(result.items[0].verified == 1);
    return 0;
}
