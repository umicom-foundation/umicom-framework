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
    if (column != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"column\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, column);
    }
    if (end_line != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"endLine\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, end_line);
    }
    if (end_column != 0U) {
        (void)umi_language_runtime_json_writer_raw(&writer, ",\"endColumn\":");
        (void)umi_language_runtime_json_writer_uint64(&writer, end_column);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "breakpointLocations", arguments, source_path, out_sequence);
}
