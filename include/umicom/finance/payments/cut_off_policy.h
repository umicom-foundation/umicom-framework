/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/payments/cut_off_policy.h
 *
 * PURPOSE:
 *   Represent rail cut-off time and deferred-execution policy using minutes from midnight.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_PAYMENTS_CUT_OFF_POLICY_H
#define UMICOM_FINANCE_PAYMENTS_CUT_OFF_POLICY_H
#include "umicom/finance/payments/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the payments cut off policy data shared with callers of this public contract.
 */
typedef struct UmiPaymentsCutOffPolicy {
    UmiFinancialId id;
    UmiFinancialId rail_id;
    uint16_t cutoff_minute;
    bool defer_after_cutoff;
} UmiPaymentsCutOffPolicy;
/**
 * Initialise payments cut off policy from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_payments_cut_off_policy_init(UmiPaymentsCutOffPolicy *value,
    const char *id,
    const char *rail_id,
    uint16_t cutoff_minute,
    bool defer_after_cutoff);
/**
 * Check that payments cut off policy satisfies its contract before another service relies
 * on it.
 */
bool umi_payments_cut_off_policy_valid(const UmiPaymentsCutOffPolicy *value);
/**
 * Provide the payments cut off policy accepts minute 600 operation used by this module and
 * its client applications.
 */
bool umi_payments_cut_off_policy_accepts_minute_600(const UmiPaymentsCutOffPolicy *value);
#ifdef __cplusplus
}
#endif
#endif
