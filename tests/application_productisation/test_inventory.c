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
#include <assert.h>

#include "umicom/application/productisation/inventory.h"

int main(void)
{
    UmiProductPortfolioInventory inventory;
    const UmiProductApplicationInventoryEntry *studio;
    const UmiProductApplicationInventoryEntry *trader;
    assert(umi_product_portfolio_inventory_build(&inventory) ==
           UMI_STATUS_OK);
    assert(inventory.application_count == 25U);
    assert(inventory.feature_count == 157U);
    /* Studio adds chat and model comparison to the canonical panel inventory. */
    assert(inventory.panel_count == 265U);
    assert(inventory.layout_count == 62U);
    studio = umi_product_portfolio_inventory_find(
        &inventory, "org.umicom.studio");
    trader = umi_product_portfolio_inventory_find(
        &inventory, "org.umicom.trader");
    assert(studio != NULL && studio->panel_count == 14U);
    assert(trader != NULL && trader->critical_panel_count >= 3U);
    assert(inventory.framework_owned_count >
           inventory.external_adapter_count);
    return 0;
}
