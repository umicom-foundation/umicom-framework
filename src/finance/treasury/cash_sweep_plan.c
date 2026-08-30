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
UmiStatus umi_treasury_cash_sweep_plan_init(UmiTreasuryCashSweepPlan *value,
    const char *id,
    int64_t requested_minor,
    int64_t maximum_minor) {
    if (value == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(value, 0, sizeof *value);
    UmiStatus status = umi_treasury_id_copy(value->id, sizeof value->id, id);
    if (status != UMI_STATUS_OK) return status;
    value->requested_minor=requested_minor;
    value->maximum_minor=maximum_minor;
    return umi_treasury_cash_sweep_plan_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
bool umi_treasury_cash_sweep_plan_valid(const UmiTreasuryCashSweepPlan *value) {
    return value != NULL && (umi_treasury_id_valid(value->id) && value->requested_minor >= 0 && value->maximum_minor >= 0);
}

int64_t umi_treasury_cash_sweep_plan_executable_minor(const UmiTreasuryCashSweepPlan *value) {
    if (value == NULL) return (int64_t)0;
    return value->requested_minor < value->maximum_minor ? value->requested_minor : value->maximum_minor;
}
