/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/debug_runtime/test_contract_adapter.c
 *
 * PURPOSE:
 *   Verify a real DAP connection is exposed through the existing stable adapter
 *   contract used by Advanced Debugging.
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

#include "umicom/debug_runtime/contract_adapter.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugRuntimeMemoryTransport *memory = NULL;
    UmiDebugRuntimeTransport transport;
    UmiDebugRuntimeAdapter *adapter = NULL;
    UmiDebugRuntimeContractAdapter *owner = NULL;
    UmiDebugAdapterDescriptor descriptor;
    UmiDebugAdapterRequest request;
    UmiDebugAdapterResponse response;
    char frame[1024];
    size_t frame_length = 0U;

    assert(umi_debug_runtime_memory_transport_create(
        &memory, &transport) == UMI_STATUS_OK);
    assert(umi_debug_runtime_adapter_create_with_transport(
        "test", &transport, &adapter) == UMI_STATUS_OK);

    assert(umi_debug_runtime_contract_adapter_create(
        adapter,
        "dap.test",
        "Test DAP",
        "gdb",
        0U,
        0U,
        &owner,
        &descriptor) == UMI_STATUS_OK);
    assert(descriptor.functions.invoke != NULL);

    assert(umi_language_runtime_frame_encode(
        "{\"seq\":2,\"type\":\"response\",\"request_seq\":1,"
        "\"success\":true,\"command\":\"threads\",\"body\":{\"threads\":[]}}",
        frame, sizeof(frame), &frame_length) == UMI_STATUS_OK);
    assert(umi_debug_runtime_memory_transport_push_read(
        memory, frame, frame_length) == UMI_STATUS_OK);

    (void)memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    request.request_id = 100U;
    request.operation = UMI_DEBUG_ADAPTER_THREADS;
    (void)strcpy(request.session_id, "session");

    assert(descriptor.functions.invoke(
        descriptor.instance,
        &request,
        NULL,
        &response) == UMI_STATUS_OK);
    assert(response.status == UMI_STATUS_OK);
    assert(response.payload_length > 0U);

    umi_debug_runtime_contract_adapter_destroy(owner);
    return 0;
}
