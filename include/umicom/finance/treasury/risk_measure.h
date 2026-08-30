/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_measure.h
 *
 * PURPOSE:
 *   Define a calculated risk measure value, confidence and horizon.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_MEASURE_H
#define UMICOM_FINANCE_TREASURY_RISK_MEASURE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRiskMeasure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t value_minor;
    uint32_t confidence_bps;
    uint32_t horizon_days;
} UmiTreasuryRiskMeasure;
UmiStatus umi_treasury_risk_measure_init(UmiTreasuryRiskMeasure *value,
    const char *id,
    int64_t value_minor,
    uint32_t confidence_bps,
    uint32_t horizon_days);
bool umi_treasury_risk_measure_valid(const UmiTreasuryRiskMeasure *value);
int64_t umi_treasury_risk_measure_absolute_value_minor(const UmiTreasuryRiskMeasure *value);
#ifdef __cplusplus
}
#endif
#endif
