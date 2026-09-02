/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_adapter_out_of_order.c
 *
 * PURPOSE:
 *   Verify a response for another outstanding request is retained, not dropped.
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

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeMemoryTransport *memory = NULL;
    UmiDebugRuntimeTransport transport;
    UmiDebugRuntimeAdapter *adapter = NULL;
    UmiDebugRuntimeEnvelope response;
    char first[1024];
    char second[1024];
    char combined[2048];
    size_t first_length = 0U;
    size_t second_length = 0U;
    uint64_t request_one = 0U;
    uint64_t request_two = 0U;

    assert(umi_debug_runtime_memory_transport_create(
        &memory, &transport) == UMI_STATUS_OK);
    assert(umi_debug_runtime_adapter_create_with_transport(
        "test", &transport, &adapter) == UMI_STATUS_OK);

    assert(umi_debug_runtime_adapter_send_request(
        adapter, "threads", NULL, "", &request_one) == UMI_STATUS_OK);
    assert(umi_debug_runtime_adapter_send_request(
        adapter, "modules", NULL, "", &request_two) == UMI_STATUS_OK);

    assert(umi_language_runtime_frame_encode(
        "{\"seq\":10,\"type\":\"response\",\"request_seq\":2,"
        "\"success\":true,\"command\":\"modules\",\"body\":{\"modules\":[]}}",
        first, sizeof(first), &first_length) == UMI_STATUS_OK);
    assert(umi_language_runtime_frame_encode(
        "{\"seq\":11,\"type\":\"response\",\"request_seq\":1,"
        "\"success\":true,\"command\":\"threads\",\"body\":{\"threads\":[]}}",
        second, sizeof(second), &second_length) == UMI_STATUS_OK);

    (void)memcpy(combined, first, first_length);
    (void)memcpy(combined + first_length, second, second_length);
    assert(umi_debug_runtime_memory_transport_push_read(
        memory,
        combined,
        first_length + second_length) == UMI_STATUS_OK);

    assert(umi_debug_runtime_adapter_wait_response(
        adapter, request_one, 0U, &response) == UMI_STATUS_OK);
    assert(response.request_sequence == request_one);

    assert(umi_debug_runtime_adapter_wait_response(
        adapter, request_two, 0U, &response) == UMI_STATUS_OK);
    assert(response.request_sequence == request_two);
    assert(strcmp(response.command, "modules") == 0);

    umi_debug_runtime_adapter_destroy(adapter);
    return 0;
}
