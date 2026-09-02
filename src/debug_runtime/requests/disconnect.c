/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/disconnect.c
 *
 * PURPOSE:
 *   Implement the DAP disconnect request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/disconnect.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request disconnect operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_disconnect(
    UmiDebugRuntimeAdapter *adapter,
    int restart,
    int terminate_debuggee,
    int suspend_debuggee,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"restart\":");
    (void)umi_language_runtime_json_writer_bool(&writer, restart);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"terminateDebuggee\":");
    (void)umi_language_runtime_json_writer_bool(&writer, terminate_debuggee);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"suspendDebuggee\":");
    (void)umi_language_runtime_json_writer_bool(&writer, suspend_debuggee);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "disconnect", arguments, "", out_sequence);
}
