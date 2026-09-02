/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_headless_surface_host.c
 *
 * PURPOSE:
 *   Verify the reusable headless host mirrors mounted, focused and refreshed
 *   application panels without creating native windows.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/presentation/presentation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost host;
    const UmiApplicationPresentationHeadlessPanel *chart;
    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.trader.standard", &runtime) == UMI_STATUS_OK);
    umi_application_presentation_headless_surface_host_init(&host);
    assert(umi_application_presentation_surface_runtime_bind_host(
               &runtime,
               umi_application_presentation_headless_surface_host_interface(&host)) ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_start(&runtime) == UMI_STATUS_OK);
    chart = umi_application_presentation_headless_surface_host_find(
        &host, "umicom.trading.chart");
    assert(chart != NULL && chart->visible);
    assert(umi_application_presentation_surface_runtime_refresh(
               &runtime, "umicom.trading.chart") == UMI_STATUS_OK);
    assert(host.event_count > host.panel_count);
    assert(umi_application_presentation_surface_runtime_stop(&runtime) == UMI_STATUS_OK);
    assert(!host.active);
    return 0;
}
