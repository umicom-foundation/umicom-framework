/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/treasury/cash_sweep_plan.c
 *
 * PURPOSE:
 *   Implement represent an executable cash sweep amount subject to a maximum.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_sweep_plan.h"
#include <string.h>
/*
 * Initialise treasury cash sweep plan from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_treasury_cash_sweep_plan_init(UmiTreasuryCashSweepPlan *value,
    const char *id,
    int64_t requested_minor,
    int64_t maximum_minor) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    value->requested_minor=requested_minor;
    value->maximum_minor=maximum_minor;
    return umi_treasury_cash_sweep_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
/*
 * Check that treasury cash sweep plan satisfies its contract before another service relies
 * on it.
 */
bool umi_treasury_cash_sweep_plan_valid(const UmiTreasuryCashSweepPlan *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->requested_minor >= 0 && value->maximum_minor >= 0);
}

/*
 * Provide the treasury cash sweep plan executable minor operation used by this module and
 * its client applications.
 */
int64_t umi_treasury_cash_sweep_plan_executable_minor(const UmiTreasuryCashSweepPlan *value) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (value == NULL) return (int64_t)0;
    return value->requested_minor < value->maximum_minor ? value->requested_minor : value->maximum_minor;
}
