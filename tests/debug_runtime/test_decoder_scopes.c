/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_scopes.c
 *
 * PURPOSE:
 *   Verify the bounded scopes DAP decoder.
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
#include "umicom/debug_runtime/decoders/scopes.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeScopeList result;
    assert(umi_debug_runtime_decode_scopes(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"scopes\",\"body\":{\"scopes\":[{\"name\":\"Locals\","
        "\"variablesReference\":23,\"expensive\":false}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(result.items[0].variables_reference == 23U);
    return 0;
}
