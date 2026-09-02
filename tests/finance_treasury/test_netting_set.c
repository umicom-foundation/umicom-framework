/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_netting_set.c
 *
 * PURPOSE:
 *   Exercise netting set validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/netting_set.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryNettingSet v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_netting_set_init(&v, "netset", 1000, 700) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_netting_set_net_minor(&v)!=300)return 2;
    return 0;
}
