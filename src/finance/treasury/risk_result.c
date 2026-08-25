/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_result.c
 *
 * PURPOSE:
 *   Implement record observed, stressed and limit risk values for governance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_result.h"
#include <string.h>
UmiStatus umi_treasury_risk_result_init(UmiTreasuryRiskResult *value,
    const char *id,
    int64_t observed_minor,
    int64_t stressed_minor,
    int64_t limit_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->observed_minor=observed_minor;
    value->stressed_minor=stressed_minor;
    value->limit_minor=limit_minor;
    return umi_treasury_risk_result_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_risk_result_valid(const UmiTreasuryRiskResult *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->limit_minor >= 0);
}

bool umi_treasury_risk_result_within_limit(const UmiTreasuryRiskResult *value) {
    if (value == NULL) return (bool)0;
    return umi_treasury_abs_i64(value->stressed_minor) <= value->limit_minor;
}
