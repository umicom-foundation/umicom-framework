/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/risk_measure.c
 *
 * PURPOSE:
 *   Implement define a calculated risk measure value, confidence and horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/risk_measure.h"
#include <string.h>
UmiStatus umi_treasury_risk_measure_init(UmiTreasuryRiskMeasure *value,
    const char *id,
    int64_t value_minor,
    uint32_t confidence_bps,
    uint32_t horizon_days) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->value_minor=value_minor;
    value->confidence_bps=confidence_bps;
    value->horizon_days=horizon_days;
    return umi_treasury_risk_measure_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_risk_measure_valid(const UmiTreasuryRiskMeasure *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->confidence_bps <= 10000U && value->horizon_days > 0U);
}

int64_t umi_treasury_risk_measure_absolute_value_minor(const UmiTreasuryRiskMeasure *value) {
    if (value == NULL) return (int64_t)0;
    return umi_treasury_abs_i64(value->value_minor);
}
