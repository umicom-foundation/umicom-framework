/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/step_in.c
 *
 * PURPOSE:
 *   Implement the DAP stepIn request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/step_in.h"

UmiStatus umi_debug_runtime_request_step_in(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "stepIn", thread_id, out_sequence);
}
