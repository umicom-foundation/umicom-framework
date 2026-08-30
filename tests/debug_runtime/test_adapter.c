/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_adapter.c
 *
 * PURPOSE:
 *   Verify a persistent DAP adapter preserves an interleaved stopped event while
 *   waiting for a matching response.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug_runtime/adapter.h"

int main(void)
{
    UmiDebugRuntimeMemoryTransport *memory = NULL;
    UmiDebugRuntimeTransport transport;
    UmiDebugRuntimeAdapter *adapter = NULL;
    UmiDebugRuntimeEnvelope response;
    UmiDebugRuntimeEnvelope event;
    char event_frame[1024];
    char response_frame[1024];
    char combined[2048];
    size_t event_length = 0U;
    size_t response_length = 0U;
    uint64_t sequence = 0U;

    assert(umi_debug_runtime_memory_transport_create(
        &memory, &transport) == UMI_STATUS_OK);
    assert(umi_debug_runtime_adapter_create_with_transport(
        "test", &transport, &adapter) == UMI_STATUS_OK);

    assert(umi_debug_runtime_adapter_send_request(
        adapter, "threads", NULL, "", &sequence) == UMI_STATUS_OK);
    assert(sequence == 1U);

    assert(umi_language_runtime_frame_encode(
        "{\"seq\":10,\"type\":\"event\",\"event\":\"stopped\","
        "\"body\":{\"reason\":\"breakpoint\"}}",
        event_frame, sizeof(event_frame), &event_length) == UMI_STATUS_OK);
    assert(umi_language_runtime_frame_encode(
        "{\"seq\":11,\"type\":\"response\",\"request_seq\":1,"
        "\"success\":true,\"command\":\"threads\",\"body\":{\"threads\":[]}}",
        response_frame, sizeof(response_frame), &response_length) ==
        UMI_STATUS_OK);

    (void)memcpy(combined, event_frame, event_length);
    (void)memcpy(combined + event_length, response_frame, response_length);
    assert(umi_debug_runtime_memory_transport_push_read(
        memory,
        combined,
        event_length + response_length) == UMI_STATUS_OK);

    assert(umi_debug_runtime_adapter_wait_response(
        adapter, sequence, 0U, &response) == UMI_STATUS_OK);
    assert(response.request_sequence == 1U);
    assert(umi_debug_runtime_adapter_next_event(
        adapter, &event) == UMI_STATUS_OK);
    assert(strcmp(event.event, "stopped") == 0);

    umi_debug_runtime_adapter_destroy(adapter);
    return 0;
}
