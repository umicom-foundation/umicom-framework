/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/set_function_breakpoints.c
 *
 * PURPOSE:
 *   Implement the DAP setFunctionBreakpoints request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/set_function_breakpoints.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_set_function_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const UmiDebugRuntimeFunctionBreakpoint *breakpoints,
    size_t breakpoint_count,
    uint64_t *out_sequence)
{
    char arguments[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;

    if (adapter == NULL ||
        (breakpoint_count > 0U && breakpoints == NULL) ||
        breakpoint_count > UMI_DEBUG_RUNTIME_MAX_BREAKPOINTS) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"breakpoints\":[");
    for (index = 0U; index < breakpoint_count; ++index) {
        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_raw(&writer, "{\"name\":");
        (void)umi_language_runtime_json_writer_string(
            &writer, breakpoints[index].name);
        if (breakpoints[index].condition[0] != '\0') {
            (void)umi_language_runtime_json_writer_raw(&writer, ",\"condition\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, breakpoints[index].condition);
        }
        if (breakpoints[index].hit_condition[0] != '\0') {
            (void)umi_language_runtime_json_writer_raw(
                &writer, ",\"hitCondition\":");
            (void)umi_language_runtime_json_writer_string(
                &writer, breakpoints[index].hit_condition);
        }
        (void)umi_language_runtime_json_writer_raw(&writer, "}");
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setFunctionBreakpoints", arguments, "", out_sequence);
}
