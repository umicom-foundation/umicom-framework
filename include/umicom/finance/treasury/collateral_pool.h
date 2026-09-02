/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_pool.h
 *
 * PURPOSE:
 *   Maintain eligible collateral inventory by identifier.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_POOL_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_POOL_H
#include "umicom/finance/treasury/collateral_asset.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury collateral pool data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCollateralPool {
    UmiTreasuryCollateralAsset items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryCollateralPool;
/**
 * Initialise treasury collateral pool from caller-provided values so later operations
 * receive a known state.
 */
void umi_treasury_collateral_pool_init(UmiTreasuryCollateralPool *registry);
/**
 * Add treasury collateral pool only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_treasury_collateral_pool_add(UmiTreasuryCollateralPool *registry, const UmiTreasuryCollateralAsset *item);
/**
 * Find treasury collateral pool while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiTreasuryCollateralAsset *umi_treasury_collateral_pool_find(const UmiTreasuryCollateralPool *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
