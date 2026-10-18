/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_decoder_threads.c
 *
 * PURPOSE:
 *   Verify the bounded threads DAP decoder.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "umicom/debug_runtime/decoders/threads.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The decoder smoke check now owns its bounded result on the heap, matching native debugger ownership and avoiding dependence on the executable stack reserve. The original test remains for review. The earlier implementation is retained below for engineering review. */
#if 0
int main(void)
{
    UmiDebugRuntimeThreadList result;
    assert(umi_debug_runtime_decode_threads(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
        "\"command\":\"threads\",\"body\":{\"threads\":[{\"id\":7,\"name\":\"main\"}]}}",
        &result) == UMI_STATUS_OK);
    assert(result.count == 1U);
    assert(result.items[0].id == 7U);
    assert(strcmp(result.items[0].name, "main") == 0);
    return 0;
}
#endif
int main(void)
{
    UmiDebugRuntimeThreadList *result = malloc(sizeof *result);
    if (result == NULL)
        return 1;
    assert(umi_debug_runtime_decode_threads(
               "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,\"success\":true,"
               "\"command\":\"threads\",\"body\":{\"threads\":[{\"id\":7,\"name\":\"main\"}]}}",
               result) == UMI_STATUS_OK);
    assert(result->count == 1U);
    assert(result->items[0].id == 7U);
    assert(strcmp(result->items[0].name, "main") == 0);
    free(result);
    return 0;
}
