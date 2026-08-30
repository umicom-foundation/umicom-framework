/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_surface_controller.c
 *
 * PURPOSE:
 *   Verify product controllers override safe Framework defaults only for the
 *   component identifiers they explicitly register.
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

static UmiStatus test_controller(
    void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    int *calls = context;
    (void)placement;
    (void)event;
    (void)payload;
    *calls += 1;
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->state = UMI_APPLICATION_PRESENTATION_STATE_EMPTY;
    return UMI_STATUS_OK;
}

int main(void)
{
    UmiApplicationPresentationSurfaceSession session;
    UmiApplicationPresentationSurfaceControllerRegistry registry;
    UmiApplicationPresentationSurfaceUpdate update;
    int calls = 0;
    assert(umi_application_presentation_surface_session_init(
               "org.umicom.workspace.trader.standard", &session) == UMI_STATUS_OK);
    umi_application_presentation_surface_controller_registry_init(&registry);
    assert(umi_application_presentation_surface_controller_register(
               &registry, "umicom.trading.chart", test_controller, &calls) ==
           UMI_STATUS_OK);
    assert(umi_application_presentation_surface_controller_dispatch(
               &registry, session.items[0].placement,
               UMI_APPLICATION_PRESENTATION_EVENT_REFRESH, NULL, &update) ==
           UMI_STATUS_OK);
    assert(calls == 1 && update.state == UMI_APPLICATION_PRESENTATION_STATE_EMPTY);
    assert(umi_application_presentation_surface_controller_register(
               &registry, "umicom.trading.chart", test_controller, &calls) ==
           UMI_STATUS_ALREADY_EXISTS);
    return 0;
}
