/*-----------------------------------------------------------------------------
 * Umicom Framework stable debugger adapter contract tests.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/adapter_contract.h"

typedef struct FakeAdapter {
    size_t invocation_count;
    uint64_t cancelled_request_id;
} FakeAdapter;

static UmiStatus fake_invoke(
    void *instance,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    FakeAdapter *adapter = (FakeAdapter *)instance;
    (void)cancellation;
    adapter->invocation_count += 1U;
    out_response->status = UMI_STATUS_OK;
    out_response->complete = 1;
    out_response->payload_length = request->payload_length;
    if (request->payload_length != 0U) {
        (void)memcpy(out_response->payload, request->payload,
                     request->payload_length);
    }
    (void)snprintf(out_response->message, sizeof(out_response->message),
                   "%s", "fake adapter completed request");
    return UMI_STATUS_OK;
}

static UmiStatus fake_cancel(void *instance, uint64_t request_id)
{
    FakeAdapter *adapter = (FakeAdapter *)instance;
    adapter->cancelled_request_id = request_id;
    return UMI_STATUS_OK;
}

static UmiDebugAdapterDescriptor make_descriptor(
    const char *id,
    int32_t priority,
    uint64_t capabilities,
    UmiDebugAdapterFlags flags,
    FakeAdapter *instance)
{
    UmiDebugAdapterDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    (void)snprintf(descriptor.id, sizeof(descriptor.id), "%s", id);
    (void)snprintf(descriptor.label, sizeof(descriptor.label), "%s", id);
    (void)snprintf(descriptor.debugger_kind,
                   sizeof(descriptor.debugger_kind), "%s", "native");
    (void)snprintf(descriptor.implementation_version,
                   sizeof(descriptor.implementation_version), "%s", "1.0");
    descriptor.capabilities = capabilities;
    descriptor.flags = flags;
    descriptor.priority = priority;
    descriptor.instance = instance;
    descriptor.functions.struct_size =
        (uint32_t)sizeof(descriptor.functions);
    descriptor.functions.api_version =
        UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    descriptor.functions.invoke = fake_invoke;
    descriptor.functions.cancel = fake_cancel;
    return descriptor;
}

int main(void)
{
    UmiDebugAdapterRegistry *registry = NULL;
    UmiDebugAdapterRegistrySnapshot snapshot;
    UmiDebugAdapterDescriptor local_descriptor;
    UmiDebugAdapterDescriptor remote_descriptor;
    UmiDebugAdapterDescriptor selected;
    UmiDebugAdapterRequest request;
    UmiDebugAdapterResponse response;
    FakeAdapter local = {0U, 0U};
    FakeAdapter remote = {0U, 0U};
    const uint64_t local_capabilities =
        UMI_DEBUG_CAP_READ_MEMORY | UMI_DEBUG_CAP_WRITE_MEMORY |
        UMI_DEBUG_CAP_DISASSEMBLE | UMI_DEBUG_CAP_REGISTERS;

    assert(umi_debug_adapter_registry_create(0U, &registry) == UMI_STATUS_OK);
    local_descriptor = make_descriptor(
        "native-local", 100, local_capabilities,
        UMI_DEBUG_ADAPTER_ENABLED | UMI_DEBUG_ADAPTER_LOCAL, &local);
    remote_descriptor = make_descriptor(
        "native-remote", 200, UMI_DEBUG_CAP_READ_MEMORY,
        UMI_DEBUG_ADAPTER_ENABLED | UMI_DEBUG_ADAPTER_REMOTE, &remote);
    assert(umi_debug_adapter_registry_register(registry, &local_descriptor) ==
           UMI_STATUS_OK);
    assert(umi_debug_adapter_registry_register(registry, &remote_descriptor) ==
           UMI_STATUS_OK);
    assert(umi_debug_adapter_registry_register(registry, &remote_descriptor) ==
           UMI_STATUS_ALREADY_EXISTS);

    assert(umi_debug_adapter_registry_select(
               registry, "native", UMI_DEBUG_CAP_WRITE_MEMORY, 1,
               &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.id, "native-local") == 0);
    assert(umi_debug_adapter_registry_select(
               registry, "native", UMI_DEBUG_CAP_READ_MEMORY, 0,
               &selected) == UMI_STATUS_OK);
    assert(strcmp(selected.id, "native-local") == 0);

    (void)memset(&request, 0, sizeof(request));
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    request.request_id = 41U;
    request.operation = UMI_DEBUG_ADAPTER_WRITE_MEMORY;
    request.payload_length = 2U;
    request.payload[0] = 0xaaU;
    request.payload[1] = 0x55U;
    assert(umi_debug_adapter_registry_invoke(
               registry, "native-local", &request, NULL, &response) ==
           UMI_STATUS_OK);
    assert(response.status == UMI_STATUS_OK);
    assert(response.complete);
    assert(response.payload_length == 2U);
    assert(response.payload[0] == 0xaaU);
    assert(local.invocation_count == 1U);

    assert(umi_debug_adapter_registry_invoke(
               registry, "native-remote", &request, NULL, &response) ==
           UMI_STATUS_UNAVAILABLE);
    assert(umi_debug_adapter_registry_cancel(
               registry, "native-local", 99U) == UMI_STATUS_OK);
    assert(local.cancelled_request_id == 99U);

    assert(umi_debug_adapter_registry_snapshot(registry, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.adapter_count == 2U);
    assert(snapshot.enabled_count == 2U);
    assert(snapshot.local_count == 1U);
    assert(snapshot.remote_count == 1U);
    umi_debug_adapter_registry_destroy(registry);
    return 0;
}
