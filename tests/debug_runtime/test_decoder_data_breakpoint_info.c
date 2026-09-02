/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_data_breakpoint_info.c
 *
 * PURPOSE:
 *   Verify the bounded data breakpoint info DAP decoder.
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
#include "umicom/debug_runtime/decoders/data_breakpoint_info.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeDataBreakpointInfo result;
    assert(umi_debug_runtime_decode_data_breakpoint_info(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"dataBreakpointInfo\",\"body\":{\"dataId\":\"x\","
        "\"description\":\"watch x\",\"accessTypes\":[\"read\",\"write\"],"
        "\"canPersist\":true}}",
        &result) == UMI_STATUS_OK);
    assert(strcmp(result.data_id, "x") == 0);
    assert(strstr(result.access_types, "write") != NULL);
    return 0;
}
