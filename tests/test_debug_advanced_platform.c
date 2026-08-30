/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_advanced_platform.c
 *
 * PURPOSE:
 *   Implement the test debug advanced platform behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework advanced debugging composition-root tests.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "umicom/debug/advanced_debugging.h"

static UmiStatus fake_invoke(
    void *instance,
    const UmiDebugAdapterRequest *request,
    const UmiDebugAdapterCancellation *cancellation,
    UmiDebugAdapterResponse *out_response)
{
    (void)instance;
    (void)request;
    (void)cancellation;
    out_response->status = UMI_STATUS_OK;
    out_response->complete = 1;
    return UMI_STATUS_OK;
}

static UmiDebugAdapterDescriptor make_adapter(void)
{
    UmiDebugAdapterDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    (void)snprintf(descriptor.id, sizeof(descriptor.id), "%s", "native-local");
    (void)snprintf(descriptor.label, sizeof(descriptor.label), "%s", "Native Local");
    (void)snprintf(descriptor.debugger_kind,
                   sizeof(descriptor.debugger_kind), "%s", "native");
    (void)snprintf(descriptor.implementation_version,
                   sizeof(descriptor.implementation_version), "%s", "1.0");
    descriptor.capabilities =
        UMI_DEBUG_CAP_READ_MEMORY | UMI_DEBUG_CAP_DISASSEMBLE |
        UMI_DEBUG_CAP_REGISTERS | UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS;
    descriptor.flags = UMI_DEBUG_ADAPTER_ENABLED | UMI_DEBUG_ADAPTER_LOCAL;
    descriptor.priority = 100;
    descriptor.functions.struct_size =
        (uint32_t)sizeof(descriptor.functions);
    descriptor.functions.api_version =
        UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    descriptor.functions.invoke = fake_invoke;
    return descriptor;
}

int main(void)
{
    UmiDebugService *service = NULL;
    UmiDebugAdvancedPlatform *platform = NULL;
    UmiDebugAdvancedPlatformSnapshot snapshot;
    UmiDebugSessionSnapshot debug_session;
    UmiDebugAdapterDescriptor adapter = make_adapter();
    UmiDebugAdapterRequest request;
    UmiDebugAdapterResponse response;
    uint64_t platform_revision;

    assert(umi_debug_service_create(&service) == UMI_STATUS_OK);
    (void)memset(&debug_session, 0, sizeof(debug_session));
    debug_session.struct_size = (uint32_t)sizeof(debug_session);
    debug_session.api_version = UMI_DEBUG_SESSION_API_VERSION;
    (void)snprintf(debug_session.id, sizeof(debug_session.id), "%s", "session-1");
    assert(umi_debug_session_registry_upsert(
               umi_debug_service_session(service), &debug_session) ==
           UMI_STATUS_OK);

    assert(umi_debug_advanced_platform_create(service, &platform) ==
           UMI_STATUS_OK);
    assert(umi_debug_advanced_platform_register_adapter(platform, &adapter) ==
           UMI_STATUS_OK);
    assert(umi_debug_advanced_platform_open_best_session(
               platform, "session-1", "native",
               UMI_DEBUG_CAP_REGISTERS, adapter.capabilities, 0) ==
           UMI_STATUS_OK);
    assert(umi_debug_inspection_session_set_state(
               umi_debug_advanced_platform_inspection(platform),
               UMI_DEBUG_INSPECTION_PAUSED) == UMI_STATUS_OK);
    assert(umi_debug_advanced_platform_refresh(platform) == UMI_STATUS_OK);
    assert(umi_debug_advanced_platform_command_enabled(
        platform, UMI_DEBUG_COMMAND_OPEN_REGISTERS));
    assert(umi_debug_advanced_platform_snapshot(platform, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.adapters.adapter_count == 1U);
    assert(snapshot.session_open);
    assert(snapshot.adapter_available);
    assert(snapshot.ready);

    (void)memset(&request, 0, sizeof(request));
    platform_revision = umi_debug_advanced_platform_revision(platform);
    assert(umi_debug_advanced_platform_invoke(
               platform, &request, NULL, &response) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_debug_advanced_platform_revision(platform) ==
           platform_revision);

    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    request.operation = UMI_DEBUG_ADAPTER_THREADS;
    assert(umi_debug_advanced_platform_invoke(
               platform, &request, NULL, &response) == UMI_STATUS_OK);
    assert(response.status == UMI_STATUS_OK);
    assert(response.complete);
    assert(umi_debug_advanced_platform_revision(platform) >
           platform_revision);

    assert(umi_debug_advanced_platform_close_session(platform) ==
           UMI_STATUS_OK);
    assert(umi_debug_advanced_platform_unregister_adapter(
               platform, "native-local") == UMI_STATUS_OK);
    umi_debug_advanced_platform_destroy(platform);
    umi_debug_service_destroy(service);
    return 0;
}
