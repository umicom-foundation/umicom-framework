/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/threads.c
 *
 * PURPOSE:
 *   Implement the DAP threads request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/threads.h"

UmiStatus umi_debug_runtime_request_threads(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "threads", out_sequence);
}
