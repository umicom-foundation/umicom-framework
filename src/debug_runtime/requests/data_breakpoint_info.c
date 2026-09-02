/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/data_breakpoint_info.c
 *
 * PURPOSE:
 *   Implement the DAP dataBreakpointInfo request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/data_breakpoint_info.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request data breakpoint info operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_request_data_breakpoint_info(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t variables_reference,
    const char *name,
    uint64_t frame_id,
    const char *bytes,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || name == NULL || name[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{");
    /* Apply this branch only when its contract condition is satisfied. */
    if (variables_reference != 0U) {
        (void)umi_language_runtime_json_writer_raw(
            &writer, "\"variablesReference\":");
        (void)umi_language_runtime_json_writer_uint64(
            &writer, variables_reference);
        (void)umi_language_runtime_json_writer_raw(&writer, ",");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "\"name\":");
    (void)umi_language_runtime_json_writer_string(&writer, name);
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (frame_id != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"frameId\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, frame_id);
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (bytes != NULL && bytes[0] != '\0') {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"bytes\":");
        (void)umi_language_runtime_json_writer_string(&writer, bytes);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "dataBreakpointInfo", arguments, name, out_sequence);
}
