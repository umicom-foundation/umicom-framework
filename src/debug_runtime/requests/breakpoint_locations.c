/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/breakpoint_locations.c
 *
 * PURPOSE:
 *   Implement the DAP breakpointLocations request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/breakpoint_locations.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request breakpoint locations operation used by this module and
 * its client applications.
 */
UmiStatus umi_debug_runtime_request_breakpoint_locations(
    UmiDebugRuntimeAdapter *adapter,
    const char *source_path,
    uint32_t line,
    uint32_t column,
    uint32_t end_line,
    uint32_t end_column,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || source_path == NULL ||
        source_path[0] == '\0' || line == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"source\":{\"path\":");
    (void)umi_language_runtime_json_writer_string(&writer, source_path);
    (void)umi_language_runtime_json_writer_raw(&writer, "},\"line\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, line);
    /* Apply this branch only when its contract condition is satisfied. */
    if (column != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, column);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (end_line != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"endLine\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, end_line);
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (end_column != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"endColumn\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, end_column);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "breakpointLocations", arguments, source_path, out_sequence);
}
