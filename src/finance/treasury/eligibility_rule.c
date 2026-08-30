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
UmiStatus umi_treasury_eligibility_rule_init(UmiTreasuryEligibilityRule *value,
    const char *id,
    int64_t minimum_value_minor,
    uint32_t maximum_maturity_days) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->minimum_value_minor=minimum_value_minor;
    value->maximum_maturity_days=maximum_maturity_days;
    return umi_treasury_eligibility_rule_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_eligibility_rule_valid(const UmiTreasuryEligibilityRule *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->minimum_value_minor >= 0 && value->maximum_maturity_days > 0U);
}

bool umi_treasury_eligibility_rule_usable(const UmiTreasuryEligibilityRule *value) {
    if (value == NULL) return (bool)0;
    return value->maximum_maturity_days > 0U;
}
