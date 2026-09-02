/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/credit_risk_snapshot.h
 *
 * PURPOSE:
 *   Capture aggregate credit exposure and expected loss.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CREDIT_RISK_SNAPSHOT_H
#define UMICOM_FINANCE_TREASURY_CREDIT_RISK_SNAPSHOT_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury credit risk snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryCreditRiskSnapshot {
    char id[UMI_TREASURY_ID_CAPACITY];
    UmiTreasuryRiskClass risk_class;
    int64_t primary_minor;
    int64_t secondary_minor;
} UmiTreasuryCreditRiskSnapshot;
/**
 * Initialise treasury credit risk snapshot from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_credit_risk_snapshot_init(UmiTreasuryCreditRiskSnapshot *value,
    const char *id,
    int64_t primary_minor,
    int64_t secondary_minor);
/**
 * Check that treasury credit risk snapshot satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_credit_risk_snapshot_valid(const UmiTreasuryCreditRiskSnapshot *value);
/**
 * Provide the treasury credit risk snapshot combined absolute minor operation used by this
 * module and its client applications.
 */
int64_t umi_treasury_credit_risk_snapshot_combined_absolute_minor(const UmiTreasuryCreditRiskSnapshot *value);
#ifdef __cplusplus
}
#endif
#endif
