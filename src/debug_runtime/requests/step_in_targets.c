/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/step_in_targets.c
 *
 * PURPOSE:
 *   Implement the DAP stepInTargets request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/step_in_targets.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_step_in_targets(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t frame_id,
    uint64_t *out_sequence)
{
    return umi_debug_runtime_request_frame(
        adapter, "stepInTargets", frame_id, out_sequence);
}
