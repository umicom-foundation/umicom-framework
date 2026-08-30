/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_exception_info.c
 *
 * PURPOSE:
 *   Verify the bounded exception info DAP decoder.
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
#include "umicom/debug_runtime/decoders/exception_info.h"

int main(void)
{
    UmiDebugRuntimeExceptionInfo result;
    assert(umi_debug_runtime_decode_exception_info(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"exceptionInfo\",\"body\":{\"exceptionId\":\"SIGSEGV\","
        "\"description\":\"Segmentation fault\",\"breakMode\":\"always\"}}",
        &result) == UMI_STATUS_OK);
    assert(strcmp(result.exception_id, "SIGSEGV") == 0);
    return 0;
}
