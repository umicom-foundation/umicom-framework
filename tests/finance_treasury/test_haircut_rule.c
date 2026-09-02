/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_haircut_rule.c
 *
 * PURPOSE:
 *   Exercise haircut rule validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/haircut_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryHaircutRule v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_haircut_rule_init(&v, "hc", 250U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_haircut_rule_remaining_bps(&v)!=9750U)return 2;
    return 0;
}
