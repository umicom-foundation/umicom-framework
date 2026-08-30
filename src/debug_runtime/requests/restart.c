/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/restart.c
 *
 * PURPOSE:
 *   Implement the DAP restart request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/restart.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_restart(
    UmiDebugRuntimeAdapter *adapter,
    const char *arguments_json,
    uint64_t *out_sequence)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_runtime_request_raw(
        adapter, "restart", arguments_json, "", out_sequence);
}
