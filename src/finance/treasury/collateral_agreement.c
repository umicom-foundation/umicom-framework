/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/collateral_agreement.c
 *
 * PURPOSE:
 *   Implement model a collateral agreement threshold and minimum transfer amount.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/collateral_agreement.h"
#include <string.h>
UmiStatus umi_treasury_collateral_agreement_init(UmiTreasuryCollateralAgreement *value,
    const char *id,
    int64_t threshold_minor,
    int64_t minimum_transfer_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->threshold_minor=threshold_minor;
    value->minimum_transfer_minor=minimum_transfer_minor;
    return umi_treasury_collateral_agreement_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_collateral_agreement_valid(const UmiTreasuryCollateralAgreement *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->threshold_minor >= 0 && value->minimum_transfer_minor >= 0);
}

int64_t umi_treasury_collateral_agreement_secured_threshold_minor(const UmiTreasuryCollateralAgreement *value) {
    if (value == NULL) return (int64_t)0;
    return value->threshold_minor + value->minimum_transfer_minor;
}
