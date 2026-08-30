/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/debug_runtime/adapter.c
 *
 * PURPOSE:
 *   Implement persistent DAP framed JSON I/O. Synchronous invoke waits only for
 *   its matching response while preserving interleaved debugger events in FIFO
 *   order for later dispatch into Framework debug models.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/debug_runtime/adapter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDebugRuntimeAdapter {
    char adapter_id[UMI_DEBUG_RUNTIME_ID_CAPACITY];
    UmiDebugRuntimeTransport transport;
    UmiDebugRuntimeFramer framer;
    UmiDebugRuntimePendingTable pending;
    UmiDebugRuntimeEventQueue *events;
    UmiDebugRuntimeResponseQueue *responses;
    UmiDebugRuntimeAdapterState state;
    uint64_t next_sequence;
    uint64_t messages_sent;
    uint64_t messages_received;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    uint64_t revision;
};

static UmiStatus copy_text(
    char *destination,
    size_t capacity,
    const char *source)
{
    size_t length;

    if (destination == NULL || capacity == 0U || source == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    length = strlen(source);
    if (length + 1U > capacity) return UMI_STATUS_CAPACITY_EXCEEDED;

    (void)memcpy(destination, source, length + 1U);
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_adapter_create_with_transport(
    const char *adapter_id,
    UmiDebugRuntimeTransport *transport,
    UmiDebugRuntimeAdapter **out_adapter)
{
    UmiDebugRuntimeAdapter *adapter;
    UmiStatus status;

    if (adapter_id == NULL || adapter_id[0] == '\0' ||
        transport == NULL || out_adapter == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_transport_validate(transport);
    if (status != UMI_STATUS_OK) return status;

    *out_adapter = NULL;

    adapter = (UmiDebugRuntimeAdapter *)calloc(1U, sizeof(*adapter));
    if (adapter == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    status = copy_text(
        adapter->adapter_id,
        sizeof(adapter->adapter_id),
        adapter_id);
    if (status != UMI_STATUS_OK) {
        free(adapter);
        return status;
    }

    adapter->transport = *transport;
    (void)memset(transport, 0, sizeof(*transport));

    umi_language_runtime_framer_init(&adapter->framer);
    umi_debug_runtime_pending_init(&adapter->pending);

    status = umi_debug_runtime_event_queue_create(&adapter->events);
    if (status == UMI_STATUS_OK) {
        status = umi_debug_runtime_response_queue_create(
            &adapter->responses);
    }
    if (status != UMI_STATUS_OK) {
        umi_debug_runtime_event_queue_destroy(adapter->events);
        adapter->transport.destroy(adapter->transport.instance);
        free(adapter);
        return status;
    }

    adapter->state = UMI_DEBUG_RUNTIME_ADAPTER_STARTING;
    adapter->next_sequence = 1U;
    adapter->revision = 1U;

    *out_adapter = adapter;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_adapter_start_process(
    const char *adapter_id,
    const char *program,
    const char *const *arguments,
    size_t argument_count,
    const char *working_directory,
    UmiDebugRuntimeAdapter **out_adapter)
{
    UmiDebugRuntimeTransport transport;
    UmiStatus status;

    (void)memset(&transport, 0, sizeof(transport));

    status = umi_debug_runtime_transport_start_process(
        program,
        arguments,
        argument_count,
        working_directory,
        &transport);
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_adapter_create_with_transport(
        adapter_id,
        &transport,
        out_adapter);

    if (status != UMI_STATUS_OK &&
        transport.instance != NULL &&
        transport.destroy != NULL) {
        transport.destroy(transport.instance);
    }

    return status;
}

void umi_debug_runtime_adapter_destroy(UmiDebugRuntimeAdapter *adapter)
{
    if (adapter == NULL) return;

    umi_debug_runtime_response_queue_destroy(adapter->responses);
    umi_debug_runtime_event_queue_destroy(adapter->events);

    if (adapter->transport.instance != NULL) {
        adapter->transport.destroy(adapter->transport.instance);
    }

    free(adapter);
}

static UmiStatus send_json(
    UmiDebugRuntimeAdapter *adapter,
    const char *json)
{
    char frame[UMI_DEBUG_RUNTIME_FRAME_CAPACITY];
    size_t frame_length = 0U;
    UmiStatus status;

    status = umi_language_runtime_frame_encode(
        json,
        frame,
        sizeof(frame),
        &frame_length);
    if (status != UMI_STATUS_OK) return status;

    status = adapter->transport.write(
        adapter->transport.instance,
        frame,
        frame_length);

    if (status == UMI_STATUS_OK) {
        adapter->messages_sent += 1U;
        adapter->bytes_sent += frame_length;
        adapter->revision += 1U;
    }

    return status;
}

UmiStatus umi_debug_runtime_adapter_send_request(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint64_t *out_sequence)
{
    char json[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    UmiDebugRuntimePendingRequest ignored;
    uint64_t sequence;
    UmiStatus status;

    if (adapter == NULL || command == NULL || command[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    sequence = adapter->next_sequence++;
    if (sequence == 0U) sequence = adapter->next_sequence++;

    status = umi_debug_runtime_build_request(
        sequence,
        command,
        arguments_json,
        json,
        sizeof(json));
    if (status != UMI_STATUS_OK) return status;

    status = umi_debug_runtime_pending_add(
        &adapter->pending,
        sequence,
        command,
        context);
    if (status != UMI_STATUS_OK) return status;

    status = send_json(adapter, json);
    if (status != UMI_STATUS_OK) {
        (void)umi_debug_runtime_pending_take(
            &adapter->pending,
            sequence,
            &ignored);
        return status;
    }

    if (out_sequence != NULL) *out_sequence = sequence;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_adapter_receive(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t timeout_ms,
    UmiDebugRuntimeEnvelope *out_envelope)
{
    unsigned char bytes[8192];
    size_t read_count = 0U;
    size_t json_length = 0U;
    char json[UMI_DEBUG_RUNTIME_JSON_CAPACITY];
    UmiStatus status;

    if (adapter == NULL || out_envelope == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (!umi_language_runtime_framer_has_message(&adapter->framer)) {
        status = adapter->transport.read(
            adapter->transport.instance,
            bytes,
            sizeof(bytes),
            timeout_ms,
            &read_count);
        if (status != UMI_STATUS_OK) return status;

        adapter->bytes_received += read_count;

        status = umi_language_runtime_framer_feed(
            &adapter->framer,
            bytes,
            read_count);
        if (status != UMI_STATUS_OK) return status;
    }

    if (!umi_language_runtime_framer_has_message(&adapter->framer)) {
        return UMI_STATUS_NOT_FOUND;
    }

    status = umi_language_runtime_framer_pop(
        &adapter->framer,
        json,
        sizeof(json),
        &json_length);
    if (status != UMI_STATUS_OK) return status;

    (void)json_length;

    status = umi_debug_runtime_message_parse(
        json,
        out_envelope);
    if (status != UMI_STATUS_OK) return status;

    if (out_envelope->kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE &&
        out_envelope->request_sequence != 0U) {
        UmiDebugRuntimePendingRequest pending;

        if (umi_debug_runtime_pending_take(
                &adapter->pending,
                out_envelope->request_sequence,
                &pending) == UMI_STATUS_OK &&
            out_envelope->command[0] == '\0') {
            (void)snprintf(
                out_envelope->command,
                sizeof(out_envelope->command),
                "%s",
                pending.command);
        }
    }

    adapter->messages_received += 1U;
    adapter->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_adapter_wait_response(
    UmiDebugRuntimeAdapter *adapter,
    uint64_t request_sequence,
    uint32_t timeout_ms,
    UmiDebugRuntimeEnvelope *out_response)
{
    uint32_t attempts = 0U;
    UmiStatus status;

    if (adapter == NULL || request_sequence == 0U ||
        out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (umi_debug_runtime_response_queue_take(
            adapter->responses,
            request_sequence,
            out_response) == UMI_STATUS_OK) {
        return out_response->success
            ? UMI_STATUS_OK
            : UMI_STATUS_UNAVAILABLE;
    }

    while (attempts < 256U) {
        UmiDebugRuntimeEnvelope envelope;

        status = umi_debug_runtime_adapter_receive(
            adapter,
            timeout_ms,
            &envelope);

        if (status == UMI_STATUS_NOT_FOUND) {
            attempts += 1U;
            continue;
        }

        if (status != UMI_STATUS_OK) return status;

        if (envelope.kind == UMI_DEBUG_RUNTIME_MESSAGE_EVENT) {
            status = umi_debug_runtime_event_queue_push(
                adapter->events,
                &envelope);
            if (status != UMI_STATUS_OK) return status;
            attempts += 1U;
            continue;
        }

        if (envelope.kind == UMI_DEBUG_RUNTIME_MESSAGE_RESPONSE) {
            if (envelope.request_sequence == request_sequence) {
                *out_response = envelope;
                return envelope.success
                    ? UMI_STATUS_OK
                    : UMI_STATUS_UNAVAILABLE;
            }

            status = umi_debug_runtime_response_queue_push(
                adapter->responses,
                &envelope);
            if (status != UMI_STATUS_OK) return status;
        }

        attempts += 1U;
    }

    return UMI_STATUS_TIMEOUT;
}

UmiStatus umi_debug_runtime_adapter_invoke(
    UmiDebugRuntimeAdapter *adapter,
    const char *command,
    const char *arguments_json,
    const char *context,
    uint32_t timeout_ms,
    UmiDebugRuntimeEnvelope *out_response)
{
    uint64_t request_sequence = 0U;
    UmiStatus status;

    if (adapter == NULL || command == NULL || out_response == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    status = umi_debug_runtime_adapter_send_request(
        adapter,
        command,
        arguments_json,
        context,
        &request_sequence);
    if (status != UMI_STATUS_OK) return status;

    return umi_debug_runtime_adapter_wait_response(
        adapter,
        request_sequence,
        timeout_ms,
        out_response);
}

UmiStatus umi_debug_runtime_adapter_next_event(
    UmiDebugRuntimeAdapter *adapter,
    UmiDebugRuntimeEnvelope *out_event)
{
    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_debug_runtime_event_queue_pop(adapter->events, out_event);
}

UmiStatus umi_debug_runtime_adapter_set_state(
    UmiDebugRuntimeAdapter *adapter,
    UmiDebugRuntimeAdapterState state)
{
    if (adapter == NULL ||
        state < UMI_DEBUG_RUNTIME_ADAPTER_STOPPED ||
        state > UMI_DEBUG_RUNTIME_ADAPTER_FAILED) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    adapter->state = state;
    adapter->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_debug_runtime_adapter_stop(
    UmiDebugRuntimeAdapter *adapter,
    uint32_t timeout_ms)
{
    UmiStatus status;

    if (adapter == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    adapter->state = UMI_DEBUG_RUNTIME_ADAPTER_STOPPING;
    adapter->revision += 1U;

    status = adapter->transport.stop(
        adapter->transport.instance,
        timeout_ms);

    adapter->state =
        status == UMI_STATUS_OK
            ? UMI_DEBUG_RUNTIME_ADAPTER_STOPPED
            : UMI_DEBUG_RUNTIME_ADAPTER_FAILED;
    adapter->revision += 1U;
    return status;
}

int umi_debug_runtime_adapter_is_running(
    UmiDebugRuntimeAdapter *adapter)
{
    return adapter != NULL &&
        adapter->transport.instance != NULL &&
        adapter->transport.is_running(adapter->transport.instance);
}

UmiStatus umi_debug_runtime_adapter_snapshot(
    const UmiDebugRuntimeAdapter *adapter,
    UmiDebugRuntimeAdapterSnapshot *out_snapshot)
{
    if (adapter == NULL || out_snapshot == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    (void)memset(out_snapshot, 0, sizeof(*out_snapshot));
    (void)snprintf(
        out_snapshot->adapter_id,
        sizeof(out_snapshot->adapter_id),
        "%s",
        adapter->adapter_id);
    out_snapshot->state = adapter->state;
    out_snapshot->next_sequence = adapter->next_sequence;
    out_snapshot->pending_requests = adapter->pending.active_count;
    out_snapshot->queued_events =
        umi_debug_runtime_event_queue_count(adapter->events);
    out_snapshot->queued_responses =
        umi_debug_runtime_response_queue_count(adapter->responses);
    out_snapshot->messages_sent = adapter->messages_sent;
    out_snapshot->messages_received = adapter->messages_received;
    out_snapshot->bytes_sent = adapter->bytes_sent;
    out_snapshot->bytes_received = adapter->bytes_received;
    out_snapshot->revision = adapter->revision;
    return UMI_STATUS_OK;
}
