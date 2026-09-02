/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_source.c
 *
 * PURPOSE:
 *   Verify the bounded source DAP decoder.
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
#include "umicom/debug_runtime/decoders/source.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeSourceResult result;
    assert(umi_debug_runtime_decode_source(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"source\",\"body\":{\"content\":\"int x;\","
        "\"mimeType\":\"text/x-c\"}}",
        &result) == UMI_STATUS_OK);
    assert(strcmp(result.content, "int x;") == 0);
    return 0;
}
