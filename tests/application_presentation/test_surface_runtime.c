/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_runtime.c
 *
 * PURPOSE:
 *   Exercise live focus, command dispatch, snapshot and shutdown through the
 *   complete toolkit-neutral application surface runtime.
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

#include "umicom/application/presentation/presentation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost host;
    UmiApplicationPresentationSurfaceSnapshot snapshot;
    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.studio.standard", &runtime) == UMI_STATUS_OK);
    umi_application_presentation_headless_surface_host_init(&host);
    assert(umi_application_presentation_surface_runtime_bind_host(
               &runtime,
               umi_application_presentation_headless_surface_host_interface(&host)) ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_start(&runtime) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_focus(
               &runtime, "umicom.ai.chat") == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_command(
               &runtime, "umicom.ai.chat", "studio.ai.new-chat") == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_deactivate(
               &runtime, "umicom.ai.chat") == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_snapshot(
               &runtime, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.ready_count <= snapshot.visible_count);
    assert(snapshot.attention_count == 0U);
    assert(snapshot.visible_count == 9U);
    assert(snapshot.focused_component_id != NULL);
    assert(strcmp(snapshot.focused_component_id, "umicom.ai.chat") != 0);
    assert(runtime.journal.count >= snapshot.visible_count);
    assert(umi_application_presentation_surface_runtime_stop(&runtime) == UMI_STATUS_OK);
    return 0;
}
