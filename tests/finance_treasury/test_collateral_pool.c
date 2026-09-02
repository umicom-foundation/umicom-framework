/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_collateral_pool.c
 *
 * PURPOSE:
 *   Verify bounded collateral pool registration and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_pool.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiTreasuryCollateralPool r; UmiTreasuryCollateralAsset item; umi_treasury_collateral_pool_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_collateral_asset_init(&item, "asset", UMI_TREASURY_COLLATERAL_CASH, 1, 1000) != UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_collateral_pool_add(&r,&item) != UMI_STATUS_OK) return 2;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_treasury_collateral_pool_find(&r,item.id) == NULL) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_collateral_pool_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
