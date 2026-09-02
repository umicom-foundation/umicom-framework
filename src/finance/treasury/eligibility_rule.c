/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/eligibility_rule.c
 *
 * PURPOSE:
 *   Implement evaluate collateral eligibility using minimum value and maximum maturity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/eligibility_rule.h"
#include <string.h>
/*
 * Initialise treasury eligibility rule from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_eligibility_rule_init(UmiTreasuryEligibilityRule *value,
    const char *id,
    int64_t minimum_value_minor,
    uint32_t maximum_maturity_days) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->minimum_value_minor=minimum_value_minor;
    value->maximum_maturity_days=maximum_maturity_days;
    return umi_treasury_eligibility_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury eligibility rule satisfies its contract before another service
 * relies on it.
 */
bool umi_treasury_eligibility_rule_valid(const UmiTreasuryEligibilityRule *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->minimum_value_minor >= 0 && value->maximum_maturity_days > 0U);
}

/*
 * Provide the treasury eligibility rule usable operation used by this module and its
 * client applications.
 */
bool umi_treasury_eligibility_rule_usable(const UmiTreasuryEligibilityRule *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (bool)0;
    return value->maximum_maturity_days > 0U;
}
