/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/securities_finance_exposure.c
 *
 * PURPOSE:
 *   Implement calculate secured and unsecured exposure for securities financing.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/securities_finance_exposure.h"
#include <string.h>
UmiStatus umi_treasury_securities_finance_exposure_init(UmiTreasurySecuritiesFinanceExposure *value,
    const char *id,
    int64_t exposure_minor,
    int64_t collateral_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->exposure_minor=exposure_minor;
    value->collateral_minor=collateral_minor;
    return umi_treasury_securities_finance_exposure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_securities_finance_exposure_valid(const UmiTreasurySecuritiesFinanceExposure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->exposure_minor >= 0 && value->collateral_minor >= 0);
}

int64_t umi_treasury_securities_finance_exposure_unsecured_minor(const UmiTreasurySecuritiesFinanceExposure *value) {
    if (value == NULL) return (int64_t)0;
    return value->exposure_minor > value->collateral_minor ? value->exposure_minor - value->collateral_minor : 0;
}
