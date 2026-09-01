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
#include <stdlib.h>

#include "umicom/application/application.h"
#include "umicom/test_runtime/check.h"

int main(void)
{
    UmiProductPortfolioInventory *inventory =
        (UmiProductPortfolioInventory *)calloc(1U, sizeof(*inventory));

    UMI_TEST_REQUIRE(inventory != NULL);
    UMI_TEST_REQUIRE(umi_product_portfolio_inventory_build(inventory) ==
                     UMI_STATUS_OK);
    UMI_TEST_REQUIRE(inventory->application_count > 0U);
    UMI_TEST_REQUIRE(umi_productisation_command_catalogue_count() > 0U);
    free(inventory);
    return 0;
}
