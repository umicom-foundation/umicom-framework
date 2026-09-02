/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_presentation/test_product_surface_portfolio.c
 *
 * PURPOSE:
 *   Verify every Framework-owned application recipe can start through the
 *   same product-surface lifecycle without application-specific host code.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/component/recipe_catalogue.h"
#include "umicom/application/presentation/product_surface.h"

#include <assert.h>
#include <string.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    size_t index;
    size_t started = 0U;

    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U;
         index < umi_application_component_recipe_catalogue_count();
         ++index) {
        const UmiApplicationComponentRecipe *recipe =
            umi_application_component_recipe_catalogue_at(index);
        UmiApplicationPresentationProductSurfaceConfig config;
        UmiApplicationPresentationProductSurface surface;
        UmiApplicationPresentationSurfaceSnapshot snapshot;

        assert(recipe != NULL);
        config.application_id = recipe->application_id;
        config.recipe_id = recipe->recipe_id;
        config.register_controllers = NULL;
        config.controller_context = NULL;
        assert(umi_application_presentation_product_surface_init(
                   &config, &surface) == UMI_STATUS_OK);
        assert(umi_application_presentation_product_surface_snapshot(
                   &surface, &snapshot) == UMI_STATUS_OK);
        assert(strcmp(snapshot.recipe_id, recipe->recipe_id) == 0);
        assert(snapshot.panel_count == recipe->slot_count);
        umi_application_presentation_product_surface_dispose(&surface);
        started += 1U;
    }
    assert(started == umi_application_component_recipe_catalogue_count());
    assert(started > 0U);
    return 0;
}
