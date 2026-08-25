/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_pool.h
 *
 * PURPOSE:
 *   Maintain eligible collateral inventory by identifier.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_POOL_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_POOL_H
#include "umicom/finance/treasury/collateral_asset.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCollateralPool {
    UmiTreasuryCollateralAsset items[UMI_TREASURY_MAX_ITEMS];
    size_t count;
} UmiTreasuryCollateralPool;
void umi_treasury_collateral_pool_init(UmiTreasuryCollateralPool *registry);
UmiStatus umi_treasury_collateral_pool_add(UmiTreasuryCollateralPool *registry, const UmiTreasuryCollateralAsset *item);
const UmiTreasuryCollateralAsset *umi_treasury_collateral_pool_find(const UmiTreasuryCollateralPool *registry, const char *id);
#ifdef __cplusplus
}
#endif
#endif
