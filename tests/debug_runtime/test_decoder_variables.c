/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_variables.c
 *
 * PURPOSE:
 *   Verify the bounded variables DAP decoder.
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
#include "umicom/debug_runtime/decoders/variables.h"

int main(void)
{
    UmiDebugRuntimeVariableList result;
    assert(umi_debug_runtime_decode_variables(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"variables\",\"body\":{\"variables\":[{\"name\":\"x\","
        "\"value\":\"42\",\"type\":\"int\",\"variablesReference\":0}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(strcmp(result.items[0].name, "x") == 0);
    assert(strcmp(result.items[0].value, "42") == 0);
    return 0;
}
