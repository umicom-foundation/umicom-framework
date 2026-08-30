/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/set_breakpoints.c
 *
 * PURPOSE:
 *   Implement the DAP setBreakpoints request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/set_breakpoints.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_set_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    const UmiDebugBreakpointSnapshot *breakpoints,
    size_t breakpoint_count,
    int source_modified,
    uint64_t *out_sequence)
{
    char arguments[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;

    if (adapter == NULL || source_path == NULL ||
        source_path[0] == '\0' ||
        (breakpoint_count > 0U && breakpoints == NULL) ||
        breakpoint_count > UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"source\":{\"path\":");
    (void)umi_language_runtime_json_writer_string(&writer, source_path);
    (void)umi_language_runtime_json_writer_raw(&writer, "},\"breakpoints\":[");

    for (index = 0U; index < breakpoint_count; ++index) {
        if (!breakpoints[index].enabled) continue;

        if (index > 0U) {
            /*
             * A skipped disabled breakpoint can make index unsuitable for comma
             * placement, so inspect writer tail instead of assuming index.
             */
            const char tail = writer.length > 0U
                ? writer.buffer[writer.length - 1U]
                : '[';
            if (tail != '[') {
                (void)umi_language_runtime_json_writer_raw(&writer, ",");
            }
        }

        (void)umi_language_runtime_json_writer_raw(&writer, "{\"line\":");
        (void)umi_language_runtime_json_writer_uint64(
            &writer, breakpoints[index].line);
        if (breakpoints[index].column != 0U) {
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"column\":");
            (void)umi_language_runtime_json_writer_uint64(
                &writer, breakpoints[index].column);
        }
        if (breakpoints[index].condition[0] != '\0') {
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"condition\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, breakpoints[index].condition);
        }
        if (breakpoints[index].log_message[0] != '\0') {
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"logMessage\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, breakpoints[index].log_message);
        }
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }

    (void)umi_language_runtime_json_writer_raw(
        &writer, "],\"sourceModified\":");
    (void)umi_language_runtime_json_writer_bool(&writer, source_modified);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setBreakpoints", arguments, source_path, out_sequence);
}
