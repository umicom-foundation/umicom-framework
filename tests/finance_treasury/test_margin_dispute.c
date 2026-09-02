/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_dispute.c
 *
 * PURPOSE:
 *   Exercise margin dispute validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_dispute.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryMarginDispute v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_margin_dispute_init(&v, "disp", 200, 50) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_margin_dispute_outstanding_minor(&v)!=150)return 2;
    return 0;
}
