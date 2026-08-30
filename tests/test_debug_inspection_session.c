/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_inspection_session.c
 *
 * PURPOSE:
 *   Implement the test debug inspection session behavior for
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
 * Umicom Framework advanced inspection session tests.
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

#include "umicom/debug/inspection_session.h"

typedef struct FakeAdapter {
    size_t invocation_count;
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
    (void)snprintf(out_response->message, sizeof(out_response->message),
                   "%s", "inspection request complete");
    return UMI_STATUS_OK;
}

static UmiDebugAdapterDescriptor make_adapter(FakeAdapter *instance)
{
    UmiDebugAdapterDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.struct_size = (uint32_t)sizeof(descriptor);
    descriptor.api_version = UMI_DEBUG_ADAPTER_CONTRACT_API_VERSION;
    (void)snprintf(descriptor.id, sizeof(descriptor.id), "%s", "test-adapter");
    (void)snprintf(descriptor.label, sizeof(descriptor.label), "%s", "Test Adapter");
    (void)snprintf(descriptor.debugger_kind,
                   sizeof(descriptor.debugger_kind), "%s", "native");
    (void)snprintf(descriptor.implementation_version,
                   sizeof(descriptor.implementation_version), "%s", "1.0");
    descriptor.capabilities =
        UMI_DEBUG_CAP_READ_MEMORY | UMI_DEBUG_CAP_WRITE_MEMORY |
        UMI_DEBUG_CAP_DISASSEMBLE | UMI_DEBUG_CAP_REGISTERS |
        UMI_DEBUG_CAP_STEP_BACK | UMI_DEBUG_CAP_DATA_BREAKPOINTS |
        UMI_DEBUG_CAP_INSTRUCTION_BREAKPOINTS;
    descriptor.flags = UMI_DEBUG_ADAPTER_ENABLED | UMI_DEBUG_ADAPTER_LOCAL;
    descriptor.priority = 100;
    descriptor.instance = instance;
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
    UmiDebugAdapterRegistry *adapters = NULL;
    UmiDebugInspectionSession *inspection = NULL;
    UmiDebugSessionSnapshot debug_session;
    UmiDebugThreadSnapshot thread;
    UmiDebugStackFrameSnapshot frame;
    UmiDebugScopeSnapshot scope;
    UmiDebugVariableSnapshot variable;
    UmiDebugMemoryBlock block;
    UmiDebugMemoryWritePlan plan;
    UmiDebugAdapterResponse response;
    UmiDebugInspectionSessionSnapshot snapshot;
    UmiDebugAdapterDescriptor adapter_descriptor;
    FakeAdapter adapter = {0U};
    unsigned char replacement[2] = {0x7fU, 0x80U};
    unsigned char value = 0U;

    assert(umi_debug_service_create(&service) == UMI_STATUS_OK);
    assert(umi_debug_adapter_registry_create(0U, &adapters) == UMI_STATUS_OK);
    adapter_descriptor = make_adapter(&adapter);
    assert(umi_debug_adapter_registry_register(adapters, &adapter_descriptor) ==
           UMI_STATUS_OK);

    (void)memset(&debug_session, 0, sizeof(debug_session));
    debug_session.struct_size = (uint32_t)sizeof(debug_session);
    debug_session.api_version = UMI_DEBUG_SESSION_API_VERSION;
    (void)snprintf(debug_session.id, sizeof(debug_session.id), "%s", "session-1");
    (void)snprintf(debug_session.adapter, sizeof(debug_session.adapter), "%s",
                   "test-adapter");
    assert(umi_debug_session_registry_upsert(
               umi_debug_service_session(service), &debug_session) ==
           UMI_STATUS_OK);

    assert(umi_debug_inspection_session_create(
               service, adapters, &inspection) == UMI_STATUS_OK);
    assert(umi_debug_inspection_session_bind(
               inspection, "session-1", "test-adapter",
               adapter_descriptor.capabilities) == UMI_STATUS_OK);
    assert(umi_debug_inspection_session_set_state(
               inspection, UMI_DEBUG_INSPECTION_PAUSED) == UMI_STATUS_OK);

    (void)memset(&thread, 0, sizeof(thread));
    thread.struct_size = (uint32_t)sizeof(thread);
    thread.api_version = UMI_DEBUG_THREAD_API_VERSION;
    (void)snprintf(thread.id, sizeof(thread.id), "%s", "thread-1");
    (void)snprintf(thread.session_id, sizeof(thread.session_id), "%s", "session-1");
    (void)snprintf(thread.name, sizeof(thread.name), "%s", "main");
    thread.current = 1;
    thread.stopped = 1;
    assert(umi_debug_thread_registry_upsert(
               umi_debug_service_thread(service), &thread) == UMI_STATUS_OK);

    (void)memset(&frame, 0, sizeof(frame));
    frame.struct_size = (uint32_t)sizeof(frame);
    frame.api_version = UMI_DEBUG_STACK_FRAME_API_VERSION;
    (void)snprintf(frame.id, sizeof(frame.id), "%s", "frame-1");
    (void)snprintf(frame.thread_id, sizeof(frame.thread_id), "%s", "thread-1");
    (void)snprintf(frame.name, sizeof(frame.name), "%s", "main");
    (void)snprintf(frame.source_uri, sizeof(frame.source_uri), "%s", "file:///main.c");
    assert(umi_debug_stack_frame_registry_upsert(
               umi_debug_service_stack_frame(service), &frame) == UMI_STATUS_OK);

    (void)memset(&scope, 0, sizeof(scope));
    scope.struct_size = (uint32_t)sizeof(scope);
    scope.api_version = UMI_DEBUG_SCOPE_API_VERSION;
    (void)snprintf(scope.id, sizeof(scope.id), "%s", "locals");
    (void)snprintf(scope.frame_id, sizeof(scope.frame_id), "%s", "frame-1");
    (void)snprintf(scope.name, sizeof(scope.name), "%s", "Locals");
    assert(umi_debug_scope_registry_upsert(
               umi_debug_service_scope(service), &scope) == UMI_STATUS_OK);

    (void)memset(&variable, 0, sizeof(variable));
    variable.struct_size = (uint32_t)sizeof(variable);
    variable.api_version = UMI_DEBUG_VARIABLE_API_VERSION;
    (void)snprintf(variable.id, sizeof(variable.id), "%s", "counter");
    (void)snprintf(variable.scope_id, sizeof(variable.scope_id), "%s", "locals");
    (void)snprintf(variable.name, sizeof(variable.name), "%s", "counter");
    (void)snprintf(variable.value, sizeof(variable.value), "%s", "10");
    assert(umi_debug_variable_registry_upsert(
               umi_debug_service_variable(service), &variable) ==
           UMI_STATUS_OK);
    assert(umi_debug_inspection_session_refresh_threads(inspection) ==
           UMI_STATUS_OK);

    (void)memset(&block, 0, sizeof(block));
    block.struct_size = (uint32_t)sizeof(block);
    block.api_version = UMI_DEBUG_MEMORY_VIEW_API_VERSION;
    (void)snprintf(block.id, sizeof(block.id), "%s", "memory-1");
    (void)snprintf(block.session_id, sizeof(block.session_id), "%s", "session-1");
    (void)snprintf(block.memory_reference,
                   sizeof(block.memory_reference), "%s", "stack");
    block.address = 0x2000U;
    block.requested_length = 8U;
    block.length = 8U;
    block.writable = 1;
    block.complete = 1;
    assert(umi_debug_memory_view_upsert(
               umi_debug_inspection_session_memory(inspection), &block) ==
           UMI_STATUS_OK);
    assert(umi_debug_memory_view_prepare_write(
               umi_debug_inspection_session_memory(inspection), "memory-1",
               2U, replacement, 2U, &plan) == UMI_STATUS_OK);
    assert(umi_debug_inspection_session_write_memory(
               inspection, &plan, NULL, &response) == UMI_STATUS_OK);
    assert(response.complete && response.status == UMI_STATUS_OK);
    assert(adapter.invocation_count == 1U);
    assert(umi_debug_memory_view_read_byte(
               umi_debug_inspection_session_memory(inspection), "memory-1",
               2U, &value) == UMI_STATUS_OK);
    assert(value == 0x7fU);

    assert(umi_debug_inspection_session_command_enabled(
        inspection, UMI_DEBUG_COMMAND_WRITE_MEMORY));
    assert(umi_debug_inspection_session_command_enabled(
        inspection, UMI_DEBUG_COMMAND_STEP_BACK));
    assert(umi_debug_inspection_session_snapshot(inspection, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.bound);
    assert(snapshot.thread_count == 1U);
    assert(snapshot.frame_count == 1U);
    assert(snapshot.scope_count == 1U);
    assert(snapshot.variable_count == 1U);
    assert(snapshot.memory_block_count == 1U);
    assert(snapshot.can_write_memory);

    umi_debug_inspection_session_destroy(inspection);
    umi_debug_adapter_registry_destroy(adapters);
    umi_debug_service_destroy(service);
    return 0;
}
