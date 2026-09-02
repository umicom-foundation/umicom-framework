/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_debug_orchestration.c
 *
 * PURPOSE:
 *   Implement the test debug orchestration behavior for
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
 * Umicom Framework
 * File: tests/test_debug_orchestration.c
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/debug/orchestration.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDebugService *service = NULL;
    UmiDebugOrchestration *orchestration = NULL;
    UmiDebugLaunchConfigurationSnapshot launch = {0};
    UmiDebugAdapterProfile adapter = {0};
    UmiDebugSessionSnapshot session = {0};
    UmiDebugBreakpointSnapshot breakpoint = {0};
    UmiDebugWatchSnapshot watch = {0};
    UmiDebugConsoleEntrySnapshot console = {0};
    UmiDebugConfigurationRequest request = {0};
    UmiDebugOrchestrationSnapshot snapshot;

    (void)strcpy(adapter.id, "gdb");
    (void)strcpy(adapter.executable, "gdb");
    adapter.enabled = 1;
    adapter.supports_launch = 1;
    (void)strcpy(launch.id, "debug-app");
    (void)strcpy(launch.adapter, "gdb");
    (void)strcpy(launch.program, "${workspaceFolder}/app");
    (void)strcpy(session.id, "session-1");
    (void)strcpy(session.configuration_id, launch.id);
    (void)strcpy(breakpoint.id, "b-1");
    (void)strcpy(breakpoint.session_id, session.id);
    (void)strcpy(breakpoint.uri, "file:///main.c");
    breakpoint.enabled = 1;
    (void)strcpy(watch.id, "w-1");
    (void)strcpy(watch.session_id, session.id);
    (void)strcpy(watch.expression, "counter");
    watch.enabled = 1;
    watch.valid = 1;
    (void)strcpy(console.id, "c-1");
    (void)strcpy(console.session_id, session.id);
    (void)strcpy(console.text, "ready");
    request.struct_size = (uint32_t)sizeof(request);
    request.api_version = UMI_DEBUG_CONFIGURATION_RESOLVER_API_VERSION;
    (void)strcpy(request.configuration_id, launch.id);
    (void)strcpy(request.workspace_folder, "/workspace/demo");

    assert(umi_debug_service_create(&service) == UMI_STATUS_OK);
    assert(umi_debug_adapter_profile_registry_upsert(
               umi_debug_service_adapter_profiles(service), &adapter) ==
           UMI_STATUS_OK);
    assert(umi_debug_launch_configuration_registry_upsert(
               umi_debug_service_launch_configuration(service), &launch) ==
           UMI_STATUS_OK);
    assert(umi_debug_session_registry_upsert(umi_debug_service_session(service),
                                             &session) == UMI_STATUS_OK);
    assert(umi_debug_breakpoint_registry_upsert(
               umi_debug_service_breakpoint(service), &breakpoint) ==
           UMI_STATUS_OK);
    assert(umi_debug_watch_registry_upsert(umi_debug_service_watch(service),
                                           &watch) == UMI_STATUS_OK);
    assert(umi_debug_console_entry_registry_upsert(
               umi_debug_service_console_entry(service), &console) ==
           UMI_STATUS_OK);
    assert(umi_debug_orchestration_create(service, NULL, NULL,
                                          &orchestration) == UMI_STATUS_OK);
    assert(umi_debug_orchestration_select_configuration(orchestration,
                                                        &request) ==
           UMI_STATUS_OK);
    assert(umi_debug_orchestration_select_session(orchestration,
                                                  session.id) == UMI_STATUS_OK);
    assert(umi_debug_orchestration_refresh(orchestration, NULL, NULL, NULL) ==
           UMI_STATUS_OK);
    assert(umi_debug_orchestration_snapshot(orchestration, &snapshot) ==
           UMI_STATUS_OK);
    assert(snapshot.configuration_ready);
    assert(snapshot.breakpoint_result_count == 1U);
    assert(snapshot.watch_result_count == 1U);
    assert(snapshot.console_result_count == 1U);
    assert(snapshot.can_start);
    assert(snapshot.can_stop);
    assert(!snapshot.can_continue);
    umi_debug_orchestration_destroy(orchestration);
    umi_debug_service_destroy(service);
    return 0;
}
