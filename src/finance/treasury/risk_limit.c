/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_limit.c
 *
 * PURPOSE:
 *   Implement define a hard treasury risk limit and warning threshold.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_limit.h"
#include <string.h>
UmiStatus umi_treasury_risk_limit_init(UmiTreasuryRiskLimit *value,
    const char *id,
    int64_t hard_limit_minor,
    int64_t warning_limit_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->hard_limit_minor=hard_limit_minor;
    value->warning_limit_minor=warning_limit_minor;
    return umi_treasury_risk_limit_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_risk_limit_valid(const UmiTreasuryRiskLimit *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->hard_limit_minor > 0 && value->warning_limit_minor >= 0 && value->warning_limit_minor <= value->hard_limit_minor);
}

int64_t umi_treasury_risk_limit_buffer_minor(const UmiTreasuryRiskLimit *value) {
    if (value == NULL) return (int64_t)0;
    return value->hard_limit_minor - value->warning_limit_minor;
}
