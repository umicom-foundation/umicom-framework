/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/funding_plan.c
 *
 * PURPOSE:
 *   Implement represent a funded amount and enforce that allocations do not exceed requirement.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/funding_plan.h"
#include <string.h>
/*
 * Initialise treasury funding plan from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_treasury_funding_plan_init(UmiTreasuryFundingPlan *value,
    const char *id,
    int64_t requirement_minor,
    int64_t allocated_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->requirement_minor=requirement_minor;
    value->allocated_minor=allocated_minor;
    return umi_treasury_funding_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury funding plan satisfies its contract before another service relies on
 * it.
 */
bool umi_treasury_funding_plan_valid(const UmiTreasuryFundingPlan *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->requirement_minor >= 0 && value->allocated_minor >= 0 && value->allocated_minor <= value->requirement_minor);
}

/*
 * Provide the treasury funding plan remaining minor operation used by this module and its
 * client applications.
 */
int64_t umi_treasury_funding_plan_remaining_minor(const UmiTreasuryFundingPlan *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->requirement_minor - value->allocated_minor;
}
