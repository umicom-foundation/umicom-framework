/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_stack_trace.c
 *
 * PURPOSE:
 *   Verify the bounded stack trace DAP decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/stack_trace.h"

int main(void)
{
    UmiDebugRuntimeStackTrace result;
    assert(umi_debug_runtime_decode_stack_trace(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"stackTrace\",\"body\":{\"stackFrames\":[{\"id\":11,"
        "\"name\":\"main\",\"source\":{\"path\":\"C:/work/main.c\"},"
        "\"line\":10,\"column\":2}],\"totalFrames\":1}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(result.items[0].id == 11U);
    assert(strcmp(result.items[0].source.path, "C:/work/main.c") == 0);
    return 0;
}
