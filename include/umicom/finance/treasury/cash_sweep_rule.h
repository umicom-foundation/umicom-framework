/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_sweep_rule.h
 *
 * PURPOSE:
 *   Define threshold-based cash sweeping between treasury accounts.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_SWEEP_RULE_H
#define UMICOM_FINANCE_TREASURY_CASH_SWEEP_RULE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury cash sweep rule data shared with callers of this public contract.
 */
typedef struct UmiTreasuryCashSweepRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t trigger_minor;
    int64_t target_minor;
} UmiTreasuryCashSweepRule;
/**
 * Initialise treasury cash sweep rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_sweep_rule_init(UmiTreasuryCashSweepRule *value,
    const char *id,
    int64_t trigger_minor,
    int64_t target_minor);
/**
 * Check that treasury cash sweep rule satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_sweep_rule_valid(const UmiTreasuryCashSweepRule *value);
/**
 * Provide the treasury cash sweep rule sweep minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_cash_sweep_rule_sweep_minor(const UmiTreasuryCashSweepRule *value);
#ifdef __cplusplus
}
#endif
#endif
