/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/request_support.c
 *
 * PURPOSE:
 *   Implement common DAP no-argument/thread/frame/reference request builders.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/request_support.h"

UmiStatus umi_debug_runtime_request_no_arguments(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t *out_sequence)
{
    if (adapter == NULL || command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_debug_runtime_adapter_send_request(
        adapter, command, NULL, "", out_sequence);
}

static UmiStatus request_numeric(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *key,
    uint64_t value,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    if (adapter == NULL || command == NULL ||
        key == NULL || value == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    umi_language_runtime_json_writer_init(
        &writer, arguments, sizeof(arguments));
    (void)umi_language_runtime_json_writer_raw(&writer, "{\"");
    (void)umi_language_runtime_json_writer_raw(&writer, key);
    (void)umi_language_runtime_json_writer_raw(&writer, "\":");
    (void)umi_language_runtime_json_writer_uint64(&writer, value);
    (void)umi_language_runtime_json_writer_raw(&writer, "}");

    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_adapter_send_request(
        adapter, command, arguments, "", out_sequence);
}

UmiStatus umi_debug_runtime_request_thread(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return request_numeric(
        adapter, command, "threadId", thread_id, out_sequence);
}

UmiStatus umi_debug_runtime_request_frame(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t frame_id,
    uint64_t *out_sequence)
{
    return request_numeric(
        adapter, command, "frameId", frame_id, out_sequence);
}

UmiStatus umi_debug_runtime_request_reference(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *key,
    uint64_t reference,
    uint64_t *out_sequence)
{
    return request_numeric(
        adapter, command, key, reference, out_sequence);
}

UmiStatus umi_debug_runtime_request_raw(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint64_t *out_sequence)
{
    if (adapter == NULL || command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_debug_runtime_adapter_send_request(
        adapter,
        command,
        arguments_json,
        context != NULL ? context : "",
        out_sequence);
}
