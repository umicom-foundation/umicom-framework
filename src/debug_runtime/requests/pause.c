/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/pause.c
 *
 * PURPOSE:
 *   Implement the DAP pause request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/pause.h"

UmiStatus umi_debug_runtime_request_pause(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "pause", thread_id, out_sequence);
}
