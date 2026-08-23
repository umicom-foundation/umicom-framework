/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_completions.c
 *
 * PURPOSE:
 *   Verify the bounded completions DAP decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/completions.h"

int main(void)
{
    UmiDebugRuntimeCompletionList result;
    assert(umi_debug_runtime_decode_completions(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"completions\",\"body\":{\"targets\":[{"
        "\"label\":\"printf\",\"text\":\"printf\"}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(strcmp(result.items[0].label, "printf") == 0);
    return 0;
}
