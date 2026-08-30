/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/step_back.c
 *
 * PURPOSE:
 *   Implement the DAP stepBack request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/step_back.h"

UmiStatus umi_debug_runtime_request_step_back(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "stepBack", thread_id, out_sequence);
}
