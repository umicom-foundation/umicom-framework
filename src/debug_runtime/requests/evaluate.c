/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/evaluate.c
 *
 * PURPOSE:
 *   Implement the DAP evaluate request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/evaluate.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_evaluate(
    UmiDebugRuntimeAdapter *adapter,
    const char *expression,
    uint64_t frame_id,
    const char *context,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || expression == NULL || expression[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"expression\":");
    (void)umi_language_runtime_json_writer_string(&writer, expression);
    if (frame_id != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"frameId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, frame_id);
    }
    if (context != NULL && context[0] != '\0') {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"context\":");
        (void)umi_language_runtime_json_writer_string(&writer, context);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "evaluate", arguments, expression, out_sequence);
}
