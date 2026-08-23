/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_source.c
 *
 * PURPOSE:
 *   Verify the bounded source DAP decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/source.h"

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
