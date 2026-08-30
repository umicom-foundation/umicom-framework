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
UmiStatus umi_treasury_funding_plan_init(UmiTreasuryFundingPlan *value,
    const char *id,
    int64_t requirement_minor,
    int64_t allocated_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->requirement_minor=requirement_minor;
    value->allocated_minor=allocated_minor;
    return umi_treasury_funding_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_funding_plan_valid(const UmiTreasuryFundingPlan *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->requirement_minor >= 0 && value->allocated_minor >= 0 && value->allocated_minor <= value->requirement_minor);
}

int64_t umi_treasury_funding_plan_remaining_minor(const UmiTreasuryFundingPlan *value) {
    if (value == NULL) return (int64_t)0;
    return value->requirement_minor - value->allocated_minor;
}
