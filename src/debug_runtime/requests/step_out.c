/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/step_out.c
 *
 * PURPOSE:
 *   Implement the DAP stepOut request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/step_out.h"

UmiStatus umi_debug_runtime_request_step_out(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "stepOut", thread_id, out_sequence);
}
