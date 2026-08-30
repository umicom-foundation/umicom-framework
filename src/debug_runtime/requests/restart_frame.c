/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/restart_frame.c
 *
 * PURPOSE:
 *   Implement the DAP restartFrame request.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/restart_frame.h"

UmiStatus umi_debug_runtime_request_restart_frame(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t frame_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_frame(
        adapter, "restartFrame", frame_id, out_sequence);
}
