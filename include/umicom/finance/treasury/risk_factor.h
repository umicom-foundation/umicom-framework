/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_factor.h
 *
 * PURPOSE:
 *   Describe a named risk factor, class and market shock in basis points.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_FACTOR_H
#define UMICOM_FINANCE_TREASURY_RISK_FACTOR_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRiskFactor {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int32_t shock_bps;
} UmiTreasuryRiskFactor;
UmiStatus umi_treasury_risk_factor_init(UmiTreasuryRiskFactor *value,
    const char *id,
    UmiTreasuryRiskClass risk_class,
    int32_t shock_bps);
bool umi_treasury_risk_factor_valid(const UmiTreasuryRiskFactor *value);
int64_t umi_treasury_risk_factor_absolute_shock_bps(const UmiTreasuryRiskFactor *value);
#ifdef __cplusplus
}
#endif
#endif
