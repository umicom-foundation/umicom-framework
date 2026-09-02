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

/*
 * Provide the debug runtime request set breakpoints operation used by this module and its
 * client applications.
 */
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

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < breakpoint_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (!breakpoints[index].enabled) continue;

        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U) {
            /*
             * A skipped disabled breakpoint can make index unsuitable for comma
             * placement, so inspect writer tail instead of assuming index.
             */
            const char tail = writer.length > 0U
                ? writer.buffer[writer.length - 1U]
                : '[';
            /* Apply this branch only when its contract condition is satisfied. */
            if (tail != '[') {
                (void)umi_language_runtime_json_writer_raw(&writer, ",");
            }
        }

        (void)umi_language_runtime_json_writer_raw(&writer, "{\"line\":");
        (void)umi_language_runtime_json_writer_uint64(
            &writer, breakpoints[index].line);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (breakpoints[index].column != 0U) {
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"column\":");
            (void)umi_language_runtime_json_writer_uint64(
                &writer, breakpoints[index].column);
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (breakpoints[index].condition[0] != '\0') {
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"condition\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, breakpoints[index].condition);
        }
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setBreakpoints", arguments, source_path, out_sequence);
}
