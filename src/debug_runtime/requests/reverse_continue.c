/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/reverse_continue.c
 *
 * PURPOSE:
 *   Implement the DAP reverseContinue request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/reverse_continue.h"

UmiStatus umi_debug_runtime_request_reverse_continue(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_thread(
        adapter, "reverseContinue", thread_id, out_sequence);
}
