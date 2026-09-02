/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/set_exception_breakpoints.c
 *
 * PURPOSE:
 *   Implement the DAP setExceptionBreakpoints request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/set_exception_breakpoints.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request set exception breakpoints operation used by this
 * module and its client applications.
 */
UmiStatus umi_debug_runtime_request_set_exception_breakpoints(
    UmiDebugRuntimeAdapter *adapter,
    const char *const *filters,
    size_t filter_count,
    uint64_t *out_sequence)
{
    char arguments[8192];
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL ||
        (filter_count > 0U && filters == NULL) ||
        filter_count > 64U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"filters\":[");
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < filter_count; ++index) {
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (filters[index] == NULL) return UMI_STATUS_INVALID_ARGUMENT;
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_string(&writer, filters[index]);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "setExceptionBreakpoints", arguments, "", out_sequence);
}
