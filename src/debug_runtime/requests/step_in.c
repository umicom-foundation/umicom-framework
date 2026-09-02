/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/step_in.c
 *
 * PURPOSE:
 *   Implement the DAP stepIn request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/step_in.h"

/*
 * Provide the debug runtime request step in operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_step_in(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "stepIn", thread_id, out_sequence);
}
