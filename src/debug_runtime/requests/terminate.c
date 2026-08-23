/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/terminate.c
 *
 * PURPOSE:
 *   Implement the DAP terminate request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/terminate.h"

UmiStatus umi_debug_runtime_request_terminate(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_no_arguments(
        adapter, "terminate", out_sequence);
}
