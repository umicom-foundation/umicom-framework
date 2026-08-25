/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_sweep_plan.c
 *
 * PURPOSE:
 *   Exercise cash sweep plan validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_sweep_plan.h"
int main(void) {
    UmiTreasuryCashSweepPlan v;
    if (umi_treasury_cash_sweep_plan_init(&v, "sweep-plan", 900, 600) != UMI_STATUS_OK) return 1;
    if(umi_treasury_cash_sweep_plan_executable_minor(&v)!=600)return 2;
    return 0;
}
