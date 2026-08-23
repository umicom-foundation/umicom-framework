/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/scopes.c
 *
 * PURPOSE:
 *   Implement the DAP scopes request.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/scopes.h"

UmiStatus umi_debug_runtime_request_scopes(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t frame_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_frame(
        adapter, "scopes", frame_id, out_sequence);
}
