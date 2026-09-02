/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/cancel.c
 *
 * PURPOSE:
 *   Implement the DAP cancel request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/cancel.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request cancel operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_cancel(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t request_id,
    uint64_t progress_id,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || (request_id == 0U && progress_id == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{");
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (request_id != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, "\"requestId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, request_id);
    }
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (progress_id != 0U) {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (request_id != 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_raw(&writer, "\"progressId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, progress_id);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "cancel", arguments, "", out_sequence);
}
