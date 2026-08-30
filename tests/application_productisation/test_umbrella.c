/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/application_productisation/test_umbrella.c
 *
 * PURPOSE:
 *   Verify the canonical application umbrella exposes productisation contracts
 *   without requiring private Framework headers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/application/application.h"

int main(void)
{
    UmiProductPortfolioInventory inventory;
    assert(umi_product_portfolio_inventory_build(&inventory) ==
           UMI_STATUS_OK);
    assert(inventory.application_count > 0U);
    assert(umi_productisation_command_catalogue_count() > 0U);
    return 0;
}
