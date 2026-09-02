/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/goto.c
 *
 * PURPOSE:
 *   Implement the DAP goto request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/goto.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request goto operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_goto(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t target_id,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || thread_id == 0U || target_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"threadId\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, thread_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"targetId\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, target_id);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "goto", arguments, "", out_sequence);
}
