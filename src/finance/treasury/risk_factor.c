/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_factor.c
 *
 * PURPOSE:
 *   Implement describe a named risk factor, class and market shock in basis points.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_factor.h"
#include <string.h>
UmiStatus umi_treasury_risk_factor_init(UmiTreasuryRiskFactor *value,
    const char *id,
    UmiTreasuryRiskClass risk_class,
    int32_t shock_bps) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->risk_class=risk_class;
    value->shock_bps=shock_bps;
    return umi_treasury_risk_factor_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_risk_factor_valid(const UmiTreasuryRiskFactor *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->risk_class >= UMI_TREASURY_RISK_MARKET && value->risk_class <= UMI_TREASURY_RISK_COUNTERPARTY);
}

int64_t umi_treasury_risk_factor_absolute_shock_bps(const UmiTreasuryRiskFactor *value) {
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64((int64_t)value->shock_bps);
}
