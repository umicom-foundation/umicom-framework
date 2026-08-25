/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_pool.c
 *
 * PURPOSE:
 *   Verify bounded collateral pool registration and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_pool.h"
int main(void) { UmiTreasuryCollateralPool r; UmiTreasuryCollateralAsset item; umi_treasury_collateral_pool_init(&r);
    if (umi_treasury_collateral_asset_init(&item, "asset", UMI_TREASURY_COLLATERAL_CASH, 1, 1000) != UMI_STATUS_OK) return 1;
    if (umi_treasury_collateral_pool_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_collateral_pool_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_collateral_pool_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
