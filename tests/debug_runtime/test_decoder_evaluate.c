/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_evaluate.c
 *
 * PURPOSE:
 *   Verify the bounded evaluate DAP decoder.
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
#include "umicom/debug_runtime/decoders/evaluate.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeEvaluateResult result;
    assert(umi_debug_runtime_decode_evaluate(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"evaluate\",\"body\":{\"result\":\"42\",\"type\":\"int\","
        "\"variablesReference\":0}}",
        &result) == UMI_STATUS_OK);
    assert(strcmp(result.result, "42") == 0);
    assert(strcmp(result.type, "int") == 0);
    return 0;
}
