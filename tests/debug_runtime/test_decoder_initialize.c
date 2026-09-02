/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_initialize.c
 *
 * PURPOSE:
 *   Verify the bounded initialize DAP decoder.
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
#include "umicom/debug_runtime/decoders/initialize.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeCapabilities result;
    assert(umi_debug_runtime_decode_initialize(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"initialize\",\"body\":{\"supportsConfigurationDoneRequest\":true,"
        "\"supportsReadMemoryRequest\":true,\"supportsDisassembleRequest\":true}}",
        &result) == UMI_STATUS_OK);
    assert(result.supports_configuration_done == 1);
    assert(result.supports_read_memory_request == 1);
    assert(result.supports_disassemble_request == 1);
    return 0;
}
