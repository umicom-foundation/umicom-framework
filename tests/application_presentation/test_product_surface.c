/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_product_surface.c
 *
 * PURPOSE:
 *   Verify one Framework product surface can select an audience layout,
 *   register controllers for every panel and forward common runtime actions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/presentation/product_surface.h"

#include <assert.h>
#include <string.h>

static UmiStatus test_controller(
    void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    size_t *event_count = (size_t *)context;
    (void)payload;
    assert(placement != NULL);
    assert(placement->panel != NULL);
    assert(out_update != NULL);
    *event_count += 1U;
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->state = event == UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT
        ? UMI_APPLICATION_PRESENTATION_STATE_DORMANT
        : UMI_APPLICATION_PRESENTATION_STATE_READY;
    return UMI_STATUS_OK;
}

static UmiStatus register_test_controllers(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    void *context)
{
    return umi_application_presentation_surface_runtime_register_controller_for_all(
        runtime, test_controller, context);
}

int main(void)
{
    UmiApplicationPresentationProductSurface surface;
    UmiApplicationPresentationProductSurfaceConfig invalid_config;
    UmiApplicationPresentationSurfaceRuntime registration_runtime;
    UmiApplicationPresentationSurfaceSnapshot snapshot;
    const char *first_component_id;
    size_t event_count = 0U;

    assert(umi_application_presentation_surface_runtime_init(
               "org.umicom.workspace.studio.standard",
               &registration_runtime) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_register_controller(
               &registration_runtime,
               registration_runtime.session.items[0].placement
                   ->panel->component_id,
               test_controller,
               &event_count) == UMI_STATUS_OK);
    assert(umi_application_presentation_surface_runtime_register_controller_for_all(
               &registration_runtime,
               test_controller,
               &event_count) == UMI_STATUS_ALREADY_EXISTS);
    assert(registration_runtime.controllers.count == 1U);

    assert(umi_application_presentation_product_surface_init_for_audience(
               "org.umicom.studio",
               UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING,
               register_test_controllers,
               &event_count,
               &surface) == UMI_STATUS_OK);
    assert(surface.initialized);
    assert(surface.audience ==
           UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_LEARNING);
    assert(strcmp(surface.application_id, "org.umicom.studio") == 0);
    assert(strcmp(surface.recipe_id,
                  "org.umicom.workspace.studio.learning") == 0);
    assert(umi_application_presentation_product_surface_snapshot(
               &surface, &snapshot) == UMI_STATUS_OK);
    assert(snapshot.panel_count > 0U);
    assert(event_count == snapshot.visible_count);

    first_component_id = surface.runtime.session.items[0].placement
                             ->panel->component_id;
    assert(umi_application_presentation_product_surface_focus(
               &surface, first_component_id) == UMI_STATUS_OK);
    assert(umi_application_presentation_product_surface_refresh(
               &surface) == UMI_STATUS_OK);
    assert(event_count > snapshot.visible_count);
    umi_application_presentation_product_surface_dispose(&surface);
    assert(!surface.initialized);
    assert(umi_application_presentation_product_surface_refresh(NULL) ==
           UMI_STATUS_INVALID_ARGUMENT);
    assert(umi_application_presentation_product_surface_refresh(&surface) ==
           UMI_STATUS_INVALID_STATE);

    invalid_config.application_id = "org.umicom.bank";
    invalid_config.recipe_id = "org.umicom.workspace.studio.standard";
    invalid_config.register_controllers = NULL;
    invalid_config.controller_context = NULL;
    assert(umi_application_presentation_product_surface_init(
               &invalid_config, &surface) == UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
