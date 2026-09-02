/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_product_panel_projection.c
 *
 * PURPOSE:
 *   Verify that Bank, TMS and Music layout panels resolve to live reusable
 *   presentation components without application-owned frontend logic.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/presentation/product_panel_projection.h"
#include "umicom/application/presentation/product_surface.h"

/* The test controller marks mounted components ready. Product applications
 * replace this with their own honest empty, ready and guarded-command states. */
static UmiStatus test_controller(
    void *context,
    const UmiApplicationPresentationPanelPlacement *placement,
    UmiApplicationPresentationSurfaceEvent event,
    const char *payload,
    UmiApplicationPresentationSurfaceUpdate *out_update)
{
    (void)context;
    (void)placement;
    (void)payload;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_update == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_update, 0, sizeof(*out_update));
    out_update->state =
        event == UMI_APPLICATION_PRESENTATION_EVENT_UNMOUNT
            ? UMI_APPLICATION_PRESENTATION_STATE_DORMANT
            : UMI_APPLICATION_PRESENTATION_STATE_READY;
    return UMI_STATUS_OK;
}

/* Register the same deterministic controller for every recipe component. */
static UmiStatus register_test_controllers(
    UmiApplicationPresentationSurfaceRuntime *runtime,
    void *context)
{
    return umi_application_presentation_surface_runtime_register_controller_for_all(
        runtime, test_controller, context);
}

/* Check one product as a complete path from recipe to panel coverage. */
static int verify_product(const char *application_id)
{
    const UmiApplicationExperienceDefinition *experience;
    UmiApplicationPresentationProductSurface surface;
    UmiApplicationProductPanelCoverage coverage;
    UmiStatus status;
    size_t index;
    experience = umi_application_experience_catalogue_find(application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return 1;
    status = umi_application_presentation_product_surface_init_for_audience(
        application_id,
        UMI_APPLICATION_COMPONENT_RECIPE_AUDIENCE_STANDARD,
        register_test_controllers,
        NULL,
        &surface);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return 2;
    status = umi_application_product_panel_coverage(
        experience, &surface.runtime, &coverage);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || coverage.panel_count != experience->panel_count ||
        coverage.bound_count != experience->panel_count ||
        coverage.unbound_count != 0U) {
        umi_application_presentation_product_surface_dispose(&surface);
        return 3;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < experience->panel_count; ++index) {
        UmiApplicationProductPanelProjection projection;
        status = umi_application_product_panel_project(
            experience, &surface.runtime, experience->panels[index].panel_id,
            &projection);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK || !projection.component_bound ||
            projection.component_id[0] == '\0' || projection.title[0] == '\0') {
            umi_application_presentation_product_surface_dispose(&surface);
            return 4;
        }
    }
    umi_application_presentation_product_surface_dispose(&surface);
    return 0;
}

/* Run the same reusable contract against three different product domains. */
int main(void)
{
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_application_product_panel_binding_count() == 0U) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if (verify_product("org.umicom.desktop") != 0) return 2;
    /* Apply this branch only when its contract condition is satisfied. */
    if (verify_product("org.umicom.os") != 0) return 3;
    /* Apply this branch only when its contract condition is satisfied. */
    if (verify_product("org.umicom.bank") != 0) return 4;
    /* Apply this branch only when its contract condition is satisfied. */
    if (verify_product("org.umicom.tms") != 0) return 5;
    /* Apply this branch only when its contract condition is satisfied. */
    if (verify_product("org.umicom.music-studio") != 0) return 6;
    return 0;
}
