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
#include <assert.h>
#include <string.h>

#include "umicom/application/productisation/workspace_guide_portfolio.h"

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
    UmiProductWorkspaceGuidePortfolio portfolio;
    const UmiProductWorkspaceGuideSummary *summary;

    umi_product_adoption_registry_init(&registry);
    assert(umi_product_adoption_registry_register(&registry, &studio) ==
           UMI_STATUS_OK);
    assert(umi_product_adoption_registry_register(&registry, &trader) ==
           UMI_STATUS_OK);
    assert(umi_product_workspace_guide_portfolio_build(
        &registry, &portfolio) == UMI_STATUS_OK);
    assert(umi_product_workspace_guide_portfolio_validate(&portfolio) ==
           UMI_STATUS_OK);
    assert(portfolio.application_count == 2U);
    assert(portfolio.layout_choice_count > 0U);
    assert(portfolio.panel_placement_count > 0U);
    assert(portfolio.acceptance_ready_count == 2U);
    assert(portfolio.average_readiness_percent <= 100U);

    summary = umi_product_workspace_guide_portfolio_find(
        &portfolio, "org.umicom.trader");
    assert(summary != NULL);
    assert(strcmp(summary->display_name, "Umicom Trader") == 0);
    assert(summary->layout_choice_count > 0U);
    assert(summary->recommended_layout_id[0] != '\0');
    assert(umi_product_workspace_guide_portfolio_at(&portfolio, 2U) == NULL);
    assert(umi_product_workspace_guide_portfolio_find(
        &portfolio, "org.umicom.missing") == NULL);
    /* Public snapshots reject out-of-range readiness instead of rendering it. */
    portfolio.applications[0].readiness_percent = 101U;
    assert(umi_product_workspace_guide_portfolio_validate(&portfolio) ==
           UMI_STATUS_INVALID_ARGUMENT);
    return 0;
}
