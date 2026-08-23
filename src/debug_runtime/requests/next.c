/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/next.c
 *
 * PURPOSE:
 *   Implement the DAP next request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/next.h"

UmiStatus umi_debug_runtime_request_next(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "next", thread_id, out_sequence);
}
