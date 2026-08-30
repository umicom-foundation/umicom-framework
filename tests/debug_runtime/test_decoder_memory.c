/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_memory.c
 *
 * PURPOSE:
 *   Verify the bounded memory DAP decoder.
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
#include "umicom/debug_runtime/decoders/memory.h"

int main(void)
{
    UmiDebugRuntimeMemoryResult result;
    assert(umi_debug_runtime_decode_memory(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"readMemory\",\"body\":{\"address\":\"0x1000\","
        "\"data\":\"AQIDBA==\"}}",
        &result) == UMI_STATUS_OK);
    assert(result.byte_count == 4U);
    assert(result.bytes[0] == 1U && result.bytes[3] == 4U);
    return 0;
}
