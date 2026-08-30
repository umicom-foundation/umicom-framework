/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/market_risk_snapshot.c
 *
 * PURPOSE:
 *   Implement capture aggregate market-risk value and stress loss.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/market_risk_snapshot.h"
#include <string.h>
UmiStatus umi_treasury_market_risk_snapshot_init(UmiTreasuryMarketRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->risk_class=UMI_TREASURY_RISK_MARKET;
    value->primary_minor=primary_minor;
    value->secondary_minor=secondary_minor;
    return umi_treasury_market_risk_snapshot_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_market_risk_snapshot_valid(const UmiTreasuryMarketRiskSnapshot *value) {
    return value != NULL && (umi_treasury_id_valid(value->id));
}

int64_t umi_treasury_market_risk_snapshot_combined_absolute_minor(const UmiTreasuryMarketRiskSnapshot *value) {
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64(value->primary_minor) + umi_treasury_abs_i64(value->secondary_minor);
}
