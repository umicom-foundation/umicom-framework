/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/set_exception_breakpoints.c
 *
 * PURPOSE:
 *   Implement the DAP setExceptionBreakpoints request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/set_exception_breakpoints.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_set_exception_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const char *const *filters,
    size_t filter_count,
    uint64_t *out_sequence)
{
    char arguments[8192];
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;

    if (adapter == NULL ||
        (filter_count > 0U && filters == NULL) ||
        filter_count > 64U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"filters\":[");
    for (index = 0U; index < filter_count; ++index) {
        if (filters[index] == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_string(&writer, filters[index]);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setExceptionBreakpoints", arguments, "", out_sequence);
}
