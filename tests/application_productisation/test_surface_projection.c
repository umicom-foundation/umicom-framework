/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_surface_projection.c
 *
 * PURPOSE:
 *   Verify every declared product panel resolves to a reusable Framework
 *   component and retains its canonical layout/context metadata.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/productisation/surface_projection.h"
#include "umicom/test_runtime/check.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiProductSurfacePortfolio *portfolio =
        (UmiProductSurfacePortfolio *)calloc(1U, sizeof(*portfolio));
    const UmiProductSurfaceProjection *editor;
    const UmiProductSurfaceProjection *profiler;
    const UmiProductSurfaceProjection *options;
    const UmiProductApplicationSurfaceSummary *studio;
    const UmiApplicationExperienceDefinition *studio_experience;
    size_t expected_surfaces = 0U;
    size_t index;

    UMI_TEST_REQUIRE(portfolio != NULL);
    UMI_TEST_REQUIRE(umi_product_surface_portfolio_build(portfolio) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(portfolio->application_count ==
                     umi_application_experience_catalogue_count());
    /* A surface exists for every canonical panel, so new panels automatically
     * extend this expectation instead of invalidating a stale literal total. */
    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UMI_TEST_REQUIRE(experience != NULL);
        expected_surfaces += experience->panel_count;
    }
    UMI_TEST_REQUIRE(portfolio->surface_count == expected_surfaces);
    UMI_TEST_REQUIRE(portfolio->covered_count == portfolio->surface_count);
    UMI_TEST_REQUIRE(portfolio->missing_count == 0U);
    editor = umi_product_surface_portfolio_find(
        portfolio, "org.umicom.studio", "editor");
    UMI_TEST_REQUIRE(editor != NULL);
    UMI_TEST_REQUIRE(editor->coverage == UMI_PRODUCT_SURFACE_COVERED);
    UMI_TEST_REQUIRE(strcmp(editor->component_id, "umicom.shared.editor") == 0);
    UMI_TEST_REQUIRE(strcmp(editor->default_region, "centre") == 0);
    /* These recently added surfaces guard against declaring a panel before its
     * reusable Framework component contract has been registered. */
    profiler = umi_product_surface_portfolio_find(
        portfolio, "org.umicom.studio", "profiler");
    options = umi_product_surface_portfolio_find(
        portfolio, "org.umicom.trader", "options-chain");
    UMI_TEST_REQUIRE(profiler != NULL &&
                     profiler->coverage == UMI_PRODUCT_SURFACE_COVERED);
    UMI_TEST_REQUIRE(options != NULL &&
                     options->coverage == UMI_PRODUCT_SURFACE_COVERED);
    studio = umi_product_surface_portfolio_application_find(
        portfolio, "org.umicom.studio");
    studio_experience = umi_application_experience_catalogue_find(
        "org.umicom.studio");
    UMI_TEST_REQUIRE(studio != NULL && studio_experience != NULL);
    UMI_TEST_REQUIRE(studio->surface_count == studio_experience->panel_count);
    UMI_TEST_REQUIRE(studio->missing_count == 0U);
    UMI_TEST_REQUIRE(
        umi_application_component_capability_count("umicom.audit") > 0U);
    UMI_TEST_REQUIRE(
        umi_application_component_capability_at("umicom.audit", 0U) != NULL);
    free(portfolio);
    return 0;
}
