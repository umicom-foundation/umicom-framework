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

/*
 * Provide the debug runtime request no arguments operation used by this module and its
 * client applications.
 */
UmiStatus umi_debug_runtime_request_no_arguments(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t *out_sequence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (adapter == NULL || command == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    return umi_debug_runtime_adapter_send_request(
        adapter, command, NULL, "", out_sequence);
}

/* Provide the request numeric operation used by this module and its client applications. */
static UmiStatus request_numeric(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *key,
    uint64_t value,
    uint64_t *out_sequence)
{
    char arguments[256];
    UmiLanguageRuntimeJsonWriter writer;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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

    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (writer.status != UMI_STATUS_OK) return writer.status;

    return umi_debug_runtime_adapter_send_request(
        adapter, command, arguments, "", out_sequence);
}

/*
 * Provide the debug runtime request thread operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_thread(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t thread_id,
    uint64_t *out_sequence)
{
    return request_numeric(
        adapter, command, "threadId", thread_id, out_sequence);
}

/*
 * Provide the debug runtime request frame operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_frame(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    uint64_t frame_id,
    uint64_t *out_sequence)
{
    return request_numeric(
        adapter, command, "frameId", frame_id, out_sequence);
}

/*
 * Provide the debug runtime request reference operation used by this module and its client
 * applications.
 */
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

/*
 * Provide the debug runtime request raw operation used by this module and its client
 * applications.
 */
UmiStatus umi_debug_runtime_request_raw(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint64_t *out_sequence)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
