/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/terminate_threads.c
 *
 * PURPOSE:
 *   Implement the DAP terminateThreads request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/terminate_threads.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_terminate_threads(
    UmiDebugRuntimeAdapter *adapter,
    const uint64_t *thread_ids,
    size_t thread_count,
    uint64_t *out_sequence)
{
    char arguments[4096];
    UmiLanguageRuntimeJsonWriter writer;
    size_t index;

    if (adapter == NULL ||
        (thread_count > 0U && thread_ids == NULL) ||
        thread_count > 256U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"threadIds\":[");
    for (index = 0U; index < thread_count; ++index) {
        if (index > 0U) {
            (void)umi_language_runtime_json_writer_raw(&writer, ",");
        }
        (void)umi_language_runtime_json_writer_uint64(
            &writer, thread_ids[index]);
    }
    (void)umi_language_runtime_json_writer_raw(&writer, "]}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "terminateThreads", arguments, "", out_sequence);
}
