/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/continue.c
 *
 * PURPOSE:
 *   Implement the DAP continue request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/continue.h"

/*
 * Provide the debug runtime request continue operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_continue(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "continue", thread_id, out_sequence);
}
