/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_runtime_safety.c
 *
 * PURPOSE:
 *   Verify read-only panels reject commands, guarded panels require a product
 *   controller and focused workspaces block commands while in the background.
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

static UmiStatus safe_controller(
    void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    (void)context;
    (void)placement;
    (void)event;
    (void)payload;
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->state = UMI_APPLICATION_PRESENTATION_STATE_READY;
    return UMI_STATUS_OK;
}

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
    UmiApplicationPresentationSurfaceRuntime guarded_trader;
    UmiApplicationPresentationSurfaceRuntime focused_studio;
    UmiApplicationPresentationHeadlessSurfaceHost host;

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.trader.standard", &trader) == UMI_STATUS_OK);
    bind_and_start(&trader, &host);
    assert(umi_application_presentation_surface_runtime_command(
               &trader, "umicom.trading.chart", "chart.change") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_application_presentation_surface_runtime_command(
               &trader, "umicom.trading.order-ticket", "order.prepare") ==
           UMI_STATUS_PERMISSION_DENIED);
    assert(umi_application_presentation_surface_runtime_stop(&trader) ==
           UMI_STATUS_OK);

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.trader.standard", &guarded_trader) ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_register_controller(
               &guarded_trader, "umicom.trading.order-ticket", safe_controller,
               NULL) == UMI_STATUS_OK);
    bind_and_start(&guarded_trader, &host);
    assert(umi_application_presentation_surface_runtime_command(
               &guarded_trader, "umicom.trading.order-ticket", "order.prepare") ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_stop(&guarded_trader) ==
           UMI_STATUS_OK);

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.studio.focus", &focused_studio) ==
           UMI_STATUS_OK);
    bind_and_start(&focused_studio, &host);
    assert(umi_application_presentation_surface_runtime_set_background(
               &focused_studio, 1) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_command(
               &focused_studio, "umicom.development.editor", "editor.save") ==
           UMI_STATUS_INVALID_STATE);
    assert(umi_application_presentation_surface_runtime_stop(&focused_studio) ==
           UMI_STATUS_OK);
    return 0;
}
