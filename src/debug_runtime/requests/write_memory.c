/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/requests/write_memory.c
 *
 * PURPOSE:
 *   Implement the DAP writeMemory request without shell interpolation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/requests/write_memory.h"

#include <string.h>
#include "umicom/language_runtime/json_writer.h"

UmiStatus umi_debug_runtime_request_write_memory(
    UmiDebugRuntimeAdapter *adapter,
    const char *memory_reference,
    int64_t offset,
    const char *base64_data,
    int allow_partial,
    uint64_t *out_sequence)
{
    char arguments[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || memory_reference == NULL ||
        memory_reference[0] == '\0' ||
        base64_data == NULL || base64_data[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(&writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(
        &writer, "{\"memoryReference\":");
    (void)umi_language_runtime_json_writer_string(&writer, memory_reference);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"offset\":");
    (void)umi_language_runtime_json_writer_int64(&writer, offset);
    (void)umi_language_runtime_json_writer_raw(
        &writer, ",\"allowPartial\":");
    (void)umi_language_runtime_json_writer_bool(&writer, allow_partial);
    (void)umi_language_runtime_json_writer_raw(&writer, ",\"data\":");
    (void)umi_language_runtime_json_writer_string(&writer, base64_data);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_request_raw(
        adapter, "writeMemory", arguments, memory_reference, out_sequence);
}
