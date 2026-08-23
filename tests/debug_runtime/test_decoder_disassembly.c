/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_disassembly.c
 *
 * PURPOSE:
 *   Verify the bounded disassembly DAP decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/disassembly.h"

int main(void)
{
    UmiDebugRuntimeDisassembly result;
    assert(umi_debug_runtime_decode_disassembly(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"disassemble\",\"body\":{\"instructions\":[{"
        "\"address\":\"0x1000\",\"instruction\":\"mov eax, 1\"}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(strcmp(result.items[0].address, "0x1000") == 0);
    return 0;
}
