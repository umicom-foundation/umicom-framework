/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_runtime_context.c
 *
 * PURPOSE:
 *   Verify shared workspace context reaches compatible panels and that
 *   checkpoint decisions follow each recipe's declared persistence policy.
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

int main(void)
{
    UmiApplicationPresentationSurfaceRuntime runtime;
    UmiApplicationPresentationHeadlessSurfaceHost host;
    uint64_t before;

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.trader.standard", &runtime) == UMI_STATUS_OK);
    umi_application_presentation_headless_surface_host_init(&host);
    assert(umi_application_presentation_surface_runtime_bind_host(
               &runtime,
               umi_application_presentation_headless_surface_host_interface(&host)) ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_start(&runtime) ==
           UMI_STATUS_OK);
    before = host.event_count;
    assert(umi_application_presentation_surface_runtime_context_changed(
               &runtime, "umicom.trading.chart", "instrument:EURUSD") ==
           UMI_STATUS_OK);
    assert(host.event_count > before);
    assert(!umi_application_presentation_surface_runtime_checkpoint_due(
        &runtime, 14U, 1));
    assert(umi_application_presentation_surface_runtime_checkpoint_due(
        &runtime, 15U, 0));
    assert(umi_application_presentation_surface_runtime_stop(&runtime) ==
           UMI_STATUS_OK);
    return 0;
}
