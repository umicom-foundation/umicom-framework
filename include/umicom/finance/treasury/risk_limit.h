/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_limit.h
 *
 * PURPOSE:
 *   Define a hard treasury risk limit and warning threshold.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_LIMIT_H
#define UMICOM_FINANCE_TREASURY_RISK_LIMIT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryRiskLimit {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t hard_limit_minor;
    int64_t warning_limit_minor;
} UmiTreasuryRiskLimit;
UmiStatus umi_treasury_risk_limit_init(UmiTreasuryRiskLimit *value,
    const char *id,
    int64_t hard_limit_minor,
    int64_t warning_limit_minor);
bool umi_treasury_risk_limit_valid(const UmiTreasuryRiskLimit *value);
int64_t umi_treasury_risk_limit_buffer_minor(const UmiTreasuryRiskLimit *value);
#ifdef __cplusplus
}
#endif
#endif
