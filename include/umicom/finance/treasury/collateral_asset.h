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
/**
 * Represent the treasury collateral asset data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCollateralAsset {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryCollateralKind kind;
    int64_t quantity;
    int64_t unit_value_minor;
} UmiTreasuryCollateralAsset;
/**
 * Initialise treasury collateral asset from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_collateral_asset_init(UmiTreasuryCollateralAsset *value,
    const char *id,
    UmiTreasuryCollateralKind kind,
    int64_t quantity,
    int64_t unit_value_minor);
/**
 * Check that treasury collateral asset satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_collateral_asset_valid(const UmiTreasuryCollateralAsset *value);
/**
 * Provide the treasury collateral asset gross value minor operation used by this module
 * and its client applications.
 */
int64_t umi_treasury_collateral_asset_gross_value_minor(const UmiTreasuryCollateralAsset *value);
#ifdef __cplusplus
}
#endif
#endif
