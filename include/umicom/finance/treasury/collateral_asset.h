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
#include "umicom/base/value_archive.h"
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
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_treasury_collateral_asset_archive_encode(const UmiTreasuryCollateralAsset *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_treasury_collateral_asset_archive_decode(const void *bytes, size_t byte_count,
    UmiTreasuryCollateralAsset *value);

#ifdef __cplusplus
}
#endif
#endif
