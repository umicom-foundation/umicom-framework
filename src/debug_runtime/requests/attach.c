/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/attach.c
 *
 * PURPOSE:
 *   Implement the DAP attach request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/attach.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request attach operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_attach(
    UmiDebugRuntimeAdapter *adapter,
    const char *arguments_json,
    uint64_t *out_sequence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || arguments_json == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    return umi_debug_runtime_request_raw(
        adapter, "attach", arguments_json, "", out_sequence);
}
