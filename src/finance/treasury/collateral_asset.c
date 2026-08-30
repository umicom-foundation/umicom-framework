/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_asset.c
 *
 * PURPOSE:
 *   Implement describe collateral inventory quantity, price and collateral kind.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_asset.h"
#include <string.h>
UmiStatus umi_treasury_collateral_asset_init(UmiTreasuryCollateralAsset *value,
    const char *id,
    UmiTreasuryCollateralKind kind,
    int64_t quantity,
    int64_t unit_value_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->kind=kind;
    value->quantity=quantity;
    value->unit_value_minor=unit_value_minor;
    return umi_treasury_collateral_asset_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_collateral_asset_valid(const UmiTreasuryCollateralAsset *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->kind >= UMI_TREASURY_COLLATERAL_CASH && value->kind <= UMI_TREASURY_COLLATERAL_SECURITY && value->quantity >= 0 && value->unit_value_minor >= 0);
}

int64_t umi_treasury_collateral_asset_gross_value_minor(const UmiTreasuryCollateralAsset *value) {
    if (value == NULL) return (int64_t)0;
    return value->quantity * value->unit_value_minor;
}
