/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_inventory.c
 *
 * PURPOSE:
 *   Verify that every canonical application, feature, panel and layout is
 *   represented by the Framework productisation inventory.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdlib.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/productisation/inventory.h"
#include "umicom/test_runtime/check.h"

int main(void)
{
    UmiProductPortfolioInventory *inventory =
        (UmiProductPortfolioInventory *)calloc(1U, sizeof(*inventory));
    const UmiProductApplicationInventoryEntry *studio;
    const UmiProductApplicationInventoryEntry *trader;
    const UmiApplicationExperienceDefinition *studio_experience;
    size_t expected_features = 0U;
    size_t expected_panels = 0U;
    size_t expected_layouts = 0U;
    size_t index;

    UMI_TEST_REQUIRE(inventory != NULL);
    UMI_TEST_REQUIRE(umi_product_portfolio_inventory_build(inventory) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(inventory->application_count ==
                     umi_application_experience_catalogue_count());

    /* Derive portfolio totals from the authoritative catalogue. This checks
     * aggregation without making legitimate future catalogue growth a failure. */
    for (index = 0U;
         index < umi_application_experience_catalogue_count();
         ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UMI_TEST_REQUIRE(experience != NULL);
        expected_features += experience->feature_count;
        expected_panels += experience->panel_count;
        expected_layouts += experience->layout_count;
    }
    UMI_TEST_REQUIRE(inventory->feature_count == expected_features);
    UMI_TEST_REQUIRE(inventory->panel_count == expected_panels);
    UMI_TEST_REQUIRE(inventory->layout_count == expected_layouts);
    studio = umi_product_portfolio_inventory_find(
        inventory, "org.umicom.studio");
    trader = umi_product_portfolio_inventory_find(
        inventory, "org.umicom.trader");
    studio_experience = umi_application_experience_catalogue_find(
        "org.umicom.studio");
    UMI_TEST_REQUIRE(studio != NULL && studio_experience != NULL);
    UMI_TEST_REQUIRE(studio->panel_count == studio_experience->panel_count);
    UMI_TEST_REQUIRE(trader != NULL && trader->critical_panel_count >= 3U);
    UMI_TEST_REQUIRE(inventory->framework_owned_count >
                     inventory->external_adapter_count);
    free(inventory);
    return 0;
}
