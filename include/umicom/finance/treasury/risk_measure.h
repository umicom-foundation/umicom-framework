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
/**
 * Represent the treasury risk measure data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRiskMeasure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t value_minor;
    uint32_t confidence_bps;
    uint32_t horizon_days;
} UmiTreasuryRiskMeasure;
/**
 * Initialise treasury risk measure from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_risk_measure_init(UmiTreasuryRiskMeasure *value,
    const char *id,
    int64_t value_minor,
    uint32_t confidence_bps,
    uint32_t horizon_days);
/**
 * Check that treasury risk measure satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_measure_valid(const UmiTreasuryRiskMeasure *value);
/**
 * Provide the treasury risk measure absolute value minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_risk_measure_absolute_value_minor(const UmiTreasuryRiskMeasure *value);
#ifdef __cplusplus
}
#endif
#endif
