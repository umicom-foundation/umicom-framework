/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_modules.c
 *
 * PURPOSE:
 *   Verify the bounded modules DAP decoder.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/modules.h"

int main(void)
{
    UmiDebugRuntimeModuleList result;
    assert(umi_debug_runtime_decode_modules(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"modules\",\"body\":{\"modules\":[{\"id\":\"1\","
        "\"name\":\"app.exe\",\"path\":\"C:/app.exe\"}],\"totalModules\":1}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(strcmp(result.items[0].name, "app.exe") == 0);
    return 0;
}
