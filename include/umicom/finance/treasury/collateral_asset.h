/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/collateral_asset.h
 *
 * PURPOSE:
 *   Describe collateral inventory quantity, price and collateral kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_COLLATERAL_ASSET_H
#define UMICOM_FINANCE_TREASURY_COLLATERAL_ASSET_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCollateralAsset {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryCollateralKind kind;
    int64_t quantity;
    int64_t unit_value_minor;
} UmiTreasuryCollateralAsset;
UmiStatus umi_treasury_collateral_asset_init(UmiTreasuryCollateralAsset *value,
    const char *id,
    UmiTreasuryCollateralKind kind,
    int64_t quantity,
    int64_t unit_value_minor);
bool umi_treasury_collateral_asset_valid(const UmiTreasuryCollateralAsset *value);
int64_t umi_treasury_collateral_asset_gross_value_minor(const UmiTreasuryCollateralAsset *value);
#ifdef __cplusplus
}
#endif
#endif
