/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/treasury/cash_sweep_plan.h
 *
 * PURPOSE:
 *   Represent an executable cash sweep amount subject to a maximum.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FINANCE_TREASURY_CASH_SWEEP_PLAN_H
#define UMICOM_FINANCE_TREASURY_CASH_SWEEP_PLAN_H
#include "umicom/finance/treasury/types.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiTreasuryCashSweepPlan {
    char id[UMI_TREASURY_ID_CAPACITY];
    int64_t requested_minor;
    int64_t maximum_minor;
} UmiTreasuryCashSweepPlan;
UmiStatus umi_treasury_cash_sweep_plan_init(UmiTreasuryCashSweepPlan *value,
    const char *id,
    int64_t requested_minor,
    int64_t maximum_minor);
bool umi_treasury_cash_sweep_plan_valid(const UmiTreasuryCashSweepPlan *value);
int64_t umi_treasury_cash_sweep_plan_executable_minor(const UmiTreasuryCashSweepPlan *value);
#ifdef __cplusplus
}
#endif
#endif
