/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/eligibility_rule.h
 *
 * PURPOSE:
 *   Evaluate collateral eligibility using minimum value and maximum maturity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_ELIGIBILITY_RULE_H
#define UMICOM_FINANCE_TREASURY_ELIGIBILITY_RULE_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryEligibilityRule {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t minimum_value_minor;
    uint32_t maximum_maturity_days;
} UmiTreasuryEligibilityRule;
UmiStatus umi_treasury_eligibility_rule_init(UmiTreasuryEligibilityRule *value,
    const char *id,
    int64_t minimum_value_minor,
    uint32_t maximum_maturity_days);
bool umi_treasury_eligibility_rule_valid(const UmiTreasuryEligibilityRule *value);
bool umi_treasury_eligibility_rule_usable(const UmiTreasuryEligibilityRule *value);
#ifdef __cplusplus
}
#endif
#endif
