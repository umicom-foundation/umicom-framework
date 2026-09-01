/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_workspace_guide_portfolio.c
 *
 * PURPOSE:
 *   Verify registered thin applications become a validated, searchable and
 *   aggregate workspace-guidance portfolio.
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

#include "umicom/application/productisation/workspace_guide_portfolio.h"
#include "umicom/test_runtime/check.h"

/* Build a representative multi-application launcher portfolio. */
int main(void)
{
    const UmiProductApplicationAdoption studio = {
        (uint32_t)sizeof(UmiProductApplicationAdoption), "studio",
        "org.umicom.studio", "Umicom Studio IDE", "umicom-studio-ide",
        UMI_PRODUCT_FRONTEND_FLAG_GTK4, 1, 1, 1, 1
    };
    const UmiProductApplicationAdoption trader = {
        (uint32_t)sizeof(UmiProductApplicationAdoption), "trader",
        "org.umicom.trader", "Umicom Trader", "umicom-trader",
        UMI_PRODUCT_FRONTEND_FLAG_GTK4, 1, 1, 1, 1
    };
    UmiProductAdoptionRegistry registry;
    UmiProductWorkspaceGuidePortfolio *portfolio =
        (UmiProductWorkspaceGuidePortfolio *)calloc(1U, sizeof(*portfolio));
    const UmiProductWorkspaceGuideSummary *summary;

    UMI_TEST_REQUIRE(portfolio != NULL);
    umi_product_adoption_registry_init(&registry);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(&registry, &studio) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_adoption_registry_register(&registry, &trader) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_portfolio_build(
        &registry, portfolio) == UMI_STATUS_OK);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_portfolio_validate(portfolio) ==
           UMI_STATUS_OK);
    UMI_TEST_REQUIRE(portfolio->application_count == registry.count);
    UMI_TEST_REQUIRE(portfolio->layout_choice_count > 0U);
    UMI_TEST_REQUIRE(portfolio->panel_placement_count > 0U);
    UMI_TEST_REQUIRE(portfolio->acceptance_ready_count == registry.count);
    UMI_TEST_REQUIRE(portfolio->average_readiness_percent <= 100U);

    summary = umi_product_workspace_guide_portfolio_find(
        portfolio, "org.umicom.trader");
    UMI_TEST_REQUIRE(summary != NULL);
    UMI_TEST_REQUIRE(strcmp(summary->display_name, "Umicom Trader") == 0);
    UMI_TEST_REQUIRE(summary->layout_choice_count > 0U);
    UMI_TEST_REQUIRE(summary->recommended_layout_id[0] != '\0');
    UMI_TEST_REQUIRE(umi_product_workspace_guide_portfolio_at(
        portfolio, registry.count) == NULL);
    UMI_TEST_REQUIRE(umi_product_workspace_guide_portfolio_find(
        portfolio, "org.umicom.missing") == NULL);
    /* Public snapshots reject out-of-range readiness instead of rendering it. */
    portfolio->applications[0].readiness_percent = 101U;
    UMI_TEST_REQUIRE(umi_product_workspace_guide_portfolio_validate(portfolio) ==
           UMI_STATUS_INVALID_ARGUMENT);
    free(portfolio);
    return 0;
}
