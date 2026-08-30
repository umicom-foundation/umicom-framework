/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_runtime_scheduling.c
 *
 * PURPOSE:
 *   Verify declared streaming and interval policies drive refresh work while
 *   reduced and paused background modes protect resources predictably.
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

static void bind_and_start(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    UmiApplicationPresentationHeadlessSurfaceHost *host)
{
    umi_application_presentation_headless_surface_host_init(host);
    assert(umi_application_presentation_surface_runtime_bind_host(
               runtime,
               umi_application_presentation_headless_surface_host_interface(host)) ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_start(runtime) ==
           UMI_STATUS_OK);
}

int main(void)
{
    UmiApplicationPresentationSurfaceRuntime trader;
    UmiApplicationPresentationSurfaceRuntime focused_studio;
    UmiApplicationPresentationHeadlessSurfaceHost trader_host;
    UmiApplicationPresentationHeadlessSurfaceHost studio_host;
    UmiApplicationPresentationSurfaceSnapshot snapshot;
    uint64_t before;

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.trader.standard", &trader) == UMI_STATUS_OK);
    bind_and_start(&trader, &trader_host);
    assert(umi_application_presentation_surface_runtime_snapshot(
               &trader, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.scheduled_refresh_count > 0U);
    assert(snapshot.streaming_count > 0U);
    before = trader_host.event_count;
    assert(umi_application_presentation_surface_runtime_advance(&trader, 1U) ==
           UMI_STATUS_OK);
    assert(trader_host.event_count > before);

    assert(umi_application_presentation_surface_runtime_set_background(
               &trader, 1) == UMI_STATUS_OK);
    before = trader_host.event_count;
    assert(umi_application_presentation_surface_runtime_advance(&trader, 1U) ==
           UMI_STATUS_OK);
    assert(trader_host.event_count > before);
    assert(umi_application_presentation_surface_runtime_stop(&trader) ==
           UMI_STATUS_OK);

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.studio.focus", &focused_studio) ==
           UMI_STATUS_OK);
    bind_and_start(&focused_studio, &studio_host);
    assert(umi_application_presentation_surface_runtime_set_background(
               &focused_studio, 1) == UMI_STATUS_OK);
    before = studio_host.event_count;
    assert(umi_application_presentation_surface_runtime_advance(
               &focused_studio, 60U) == UMI_STATUS_OK);
    assert(studio_host.event_count == before);
    assert(umi_application_presentation_surface_runtime_stop(&focused_studio) ==
           UMI_STATUS_OK);
    return 0;
}
