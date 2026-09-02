/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/settlement_failure.h
 *
 * PURPOSE:
 *   Record failed settlement exposure, age and retry eligibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_SETTLEMENT_FAILURE_H
#define UMICOM_FINANCE_TREASURY_SETTLEMENT_FAILURE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury settlement failure data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasurySettlementFailure {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t exposure_minor;
    uint32_t age_days;
    uint32_t retry_count;
} UmiTreasurySettlementFailure;
/**
 * Initialise treasury settlement failure from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_settlement_failure_init(UmiTreasurySettlementFailure *value,
    const char *id,
    int64_t exposure_minor,
    uint32_t age_days,
    uint32_t retry_count);
/**
 * Check that treasury settlement failure satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_settlement_failure_valid(const UmiTreasurySettlementFailure *value);
/**
 * Provide the treasury settlement failure aged operation used by this module and its
 * client applications.
 */
bool umi_treasury_settlement_failure_aged(const UmiTreasurySettlementFailure *value);
#ifdef __cplusplus
}
#endif
#endif
