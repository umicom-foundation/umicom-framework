/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/cancel.c
 *
 * PURPOSE:
 *   Implement the DAP cancel request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/cancel.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_cancel(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t request_id,
    uint64_t progress_id,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || (request_id == 0U && progress_id == 0U)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{");
    if (request_id != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, "\"requestId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, request_id);
    }
    if (progress_id != 0U) {
        if (request_id != 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_raw(&writer, "\"progressId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, progress_id);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "cancel", arguments, "", out_sequence);
}
