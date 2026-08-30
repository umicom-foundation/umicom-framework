/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/completions.c
 *
 * PURPOSE:
 *   Implement the DAP completions request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/completions.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_completions(
    UmiDebugRuntimeAdapter *adapter,
    const char *text,
    uint32_t column,
    uint64_t frame_id,
    uint32_t line,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || text == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"text\":");
    (void)umi_language_runtime_json_writer_string(&writer, text);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, column);
    if (frame_id != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"frameId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, frame_id);
    }
    if (line != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"line\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, line);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "completions", arguments, text, out_sequence);
}
