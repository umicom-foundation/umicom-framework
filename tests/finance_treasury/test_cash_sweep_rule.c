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
int main(void) {
    UmiTreasuryCashSweepRule v;
    if (umi_treasury_cash_sweep_rule_init(&v, "sweep-rule", 1000, 250) != UMI_STATUS_OK) return 1;
    if(umi_treasury_cash_sweep_rule_sweep_minor(&v)!=750)return 2;
    return 0;
}
