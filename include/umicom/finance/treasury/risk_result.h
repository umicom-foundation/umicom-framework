/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_result.h
 *
 * PURPOSE:
 *   Record observed, stressed and limit risk values for governance.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_RESULT_H
#define UMICOM_FINANCE_TREASURY_RISK_RESULT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRiskResult {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t observed_minor;
    int64_t stressed_minor;
    int64_t limit_minor;
} UmiTreasuryRiskResult;
UmiStatus umi_treasury_risk_result_init(UmiTreasuryRiskResult *value,
    const char *id,
    int64_t observed_minor,
    int64_t stressed_minor,
    int64_t limit_minor);
bool umi_treasury_risk_result_valid(const UmiTreasuryRiskResult *value);
bool umi_treasury_risk_result_within_limit(const UmiTreasuryRiskResult *value);
#ifdef __cplusplus
}
#endif
#endif
