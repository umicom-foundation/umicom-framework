/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/eligibility_rule.h
 *
 * PURPOSE:
 *   Evaluate collateral eligibility using minimum value and maximum maturity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_ELIGIBILITY_RULE_H
#define UMICOM_FINANCE_TREASURY_ELIGIBILITY_RULE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the treasury eligibility rule data shared with callers of this public
 * contract.
 */
typedef struct UmiTreasuryEligibilityRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t minimum_value_minor;
    uint32_t maximum_maturity_days;
} UmiTreasuryEligibilityRule;
/**
 * Initialise treasury eligibility rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_eligibility_rule_init(UmiTreasuryEligibilityRule *value,
    const char *id,
    int64_t minimum_value_minor,
    uint32_t maximum_maturity_days);
/**
 * Check that treasury eligibility rule satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_eligibility_rule_valid(const UmiTreasuryEligibilityRule *value);
/**
 * Provide the treasury eligibility rule usable operation used by this module and its
 * client applications.
 */
bool umi_treasury_eligibility_rule_usable(const UmiTreasuryEligibilityRule *value);
#ifdef __cplusplus
}
#endif
#endif
