/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_workspace.c
 *
 * PURPOSE:
 *   Verify professional debugger selection, filtering and mutable workspace
 *   operations without depending on a graphical toolkit or debug adapter.
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

#include "umicom/debug/workspace.h"

int main(void)
{
    UmiProtocolTransport *transport = NULL;
    UmiProtocolClient *client = NULL;
    UmiDapClient dap;
    UmiDebugService *service = NULL;
    UmiDebugController *controller = NULL;
    UmiDebugWorkspace *workspace = NULL;
    UmiDebugThreadSnapshot thread = {0};
    UmiDebugStackFrameSnapshot frame = {0};
    UmiDebugScopeSnapshot scope = {0};
    UmiDebugVariableSnapshot variable = {0};
    UmiDebugBreakpointSnapshot breakpoint = {0};
    UmiDebugConsoleEntrySnapshot console = {0};
    UmiDebugWorkspaceSnapshot snapshot;
    char watch_id[128];

    assert(umi_protocol_transport_create_memory(16U, &transport) ==
           UMI_STATUS_OK);
    assert(umi_protocol_client_create(transport, &client) == UMI_STATUS_OK);
    assert(umi_protocol_client_start(client) == UMI_STATUS_OK);
    assert(umi_dap_client_init(&dap, client) == UMI_STATUS_OK);
    assert(umi_debug_service_create(&service) == UMI_STATUS_OK);
    assert(umi_debug_controller_create(&dap, service, &controller) ==
           UMI_STATUS_OK);
    assert(umi_debug_workspace_create(service, controller, &workspace) ==
           UMI_STATUS_OK);

    (void)strcpy(thread.id, "thread-main");
    (void)strcpy(thread.name, "Main Thread");
    thread.current = 1;
    thread.stopped = 1;
    assert(umi_debug_thread_registry_upsert(
               umi_debug_service_thread(service), &thread) == UMI_STATUS_OK);

    (void)strcpy(frame.id, "frame-main");
    (void)strcpy(frame.thread_id, thread.id);
    (void)strcpy(frame.name, "main");
    (void)strcpy(frame.source_uri, "file:///workspace/main.c");
    frame.line = 42U;
    assert(umi_debug_stack_frame_registry_upsert(
               umi_debug_service_stack_frame(service), &frame) ==
           UMI_STATUS_OK);

    (void)strcpy(scope.id, "scope-locals");
    (void)strcpy(scope.frame_id, frame.id);
    (void)strcpy(scope.name, "Locals");
    assert(umi_debug_scope_registry_upsert(
               umi_debug_service_scope(service), &scope) == UMI_STATUS_OK);

    (void)strcpy(variable.id, "variable-counter");
    (void)strcpy(variable.scope_id, scope.id);
    (void)strcpy(variable.name, "counter");
    (void)strcpy(variable.value, "7");
    (void)strcpy(variable.type, "int");
    assert(umi_debug_variable_registry_upsert(
               umi_debug_service_variable(service), &variable) ==
           UMI_STATUS_OK);

    (void)strcpy(breakpoint.id, "main.c:42:0");
    (void)strcpy(breakpoint.uri, "file:///workspace/main.c");
    breakpoint.line = 42U;
    breakpoint.enabled = 1;
    assert(umi_debug_breakpoint_registry_upsert(
               umi_debug_service_breakpoint(service), &breakpoint) ==
           UMI_STATUS_OK);

    (void)strcpy(console.id, "console-1");
    (void)strcpy(console.category, "stdout");
    (void)strcpy(console.text, "Ready to debug");
    assert(umi_debug_console_entry_registry_upsert(
               umi_debug_service_console_entry(service), &console) ==
           UMI_STATUS_OK);

    assert(umi_debug_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(strcmp(snapshot.selected_thread_id, thread.id) == 0);
    assert(strcmp(snapshot.selected_frame_id, frame.id) == 0);
    assert(strcmp(snapshot.selected_scope_id, scope.id) == 0);
    assert(snapshot.visible_variable_count == 1U);
    assert(snapshot.can_start);

    assert(umi_debug_workspace_add_watch(workspace, "counter + 1", watch_id,
                                         sizeof(watch_id)) == UMI_STATUS_OK);
    assert(watch_id[0] != '\0');
    assert(umi_debug_workspace_set_breakpoint_enabled(
               workspace, breakpoint.id, 0) == UMI_STATUS_OK);
    assert(umi_debug_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.watch_count == 1U);
    assert(snapshot.breakpoint_count == 1U);
    assert(snapshot.console_entry_count == 1U);

    assert(umi_debug_workspace_remove_watch(workspace, watch_id) ==
           UMI_STATUS_OK);
    umi_debug_workspace_clear_console(workspace);
    assert(umi_debug_workspace_snapshot(workspace, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.watch_count == 0U);
    assert(snapshot.console_entry_count == 0U);

    umi_debug_workspace_destroy(workspace);
    umi_debug_controller_destroy(controller);
    umi_debug_service_destroy(service);
    umi_protocol_client_destroy(client);
    umi_protocol_transport_destroy(transport);
    return 0;
}
