/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/risk_limit.h
 *
 * PURPOSE:
 *   Define a hard treasury risk limit and warning threshold.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_RISK_LIMIT_H
#define UMICOM_FINANCE_TREASURY_RISK_LIMIT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury risk limit data shared with callers of this public contract.
 */
typedef struct UmiTreasuryRiskLimit {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t hard_limit_minor;
    int64_t warning_limit_minor;
} UmiTreasuryRiskLimit;
/**
 * Initialise treasury risk limit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_treasury_risk_limit_init(UmiTreasuryRiskLimit *value,
    const char *id,
    int64_t hard_limit_minor,
    int64_t warning_limit_minor);
/**
 * Check that treasury risk limit satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_risk_limit_valid(const UmiTreasuryRiskLimit *value);
/**
 * Provide the treasury risk limit buffer minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_risk_limit_buffer_minor(const UmiTreasuryRiskLimit *value);
#ifdef __cplusplus
}
#endif
#endif
