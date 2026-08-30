/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_asset.c
 *
 * PURPOSE:
 *   Exercise collateral asset validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_asset.h"
int main(void) {
    UmiTreasuryCollateralAsset v;
    if (umi_treasury_collateral_asset_init(&v, "bond", UMI_TREASURY_COLLATERAL_SECURITY, 10, 100) != UMI_STATUS_OK) return 1;
    if(umi_treasury_collateral_asset_gross_value_minor(&v)!=1000)return 2;
    return 0;
}
