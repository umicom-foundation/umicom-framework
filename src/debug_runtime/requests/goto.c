/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/goto.c
 *
 * PURPOSE:
 *   Implement the DAP goto request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/goto.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_goto(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t thread_id,
    uint64_t target_id,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || thread_id == 0U || target_id == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"threadId\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, thread_id);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"targetId\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, target_id);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "goto", arguments, "", out_sequence);
}
