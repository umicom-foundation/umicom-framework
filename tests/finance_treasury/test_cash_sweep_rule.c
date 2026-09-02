/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_cash_sweep_rule.c
 *
 * PURPOSE:
 *   Exercise cash sweep rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/cash_sweep_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryCashSweepRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_cash_sweep_rule_init(&v, "sweep-rule", 1000, 250) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_cash_sweep_rule_sweep_minor(&v)!=750)return 2;
    return 0;
}
