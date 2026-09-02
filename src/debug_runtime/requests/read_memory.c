/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/read_memory.c
 *
 * PURPOSE:
 *   Implement the DAP readMemory request without shell interpolation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/read_memory.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

/*
 * Provide the debug runtime request read memory operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_read_memory(
    UmiDebugRuntimeAdapter *adapter,
    const char *memory_reference,
    int64_t offset,
    uint32_t count,
    uint64_t *out_sequence)
{
    char arguments[1024];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || memory_reference == NULL ||
        memory_reference[0] == '\0' || count == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"memoryReference\":");
    (void)umi_language_runtime_json_writer_string(&writer, memory_reference);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"offset\":");
    (void)umi_language_runtime_json_writer_int64(&writer, offset);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"count\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, count);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "readMemory", arguments, memory_reference, out_sequence);
}
