/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/set_expression.c
 *
 * PURPOSE:
 *   Implement the DAP setExpression request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/set_expression.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request set expression operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_set_expression(
    UmiDebugRuntimeAdapter *adapter,
    const char *expression,
    const char *value,
    uint64_t frame_id,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || expression == NULL || expression[0] == '\0' ||
        value == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"expression\":");
    (void)umi_language_runtime_json_writer_string(&writer, expression);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"value\":");
    (void)umi_language_runtime_json_writer_string(&writer, value);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (frame_id != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"frameId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, frame_id);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setExpression", arguments, expression, out_sequence);
}
