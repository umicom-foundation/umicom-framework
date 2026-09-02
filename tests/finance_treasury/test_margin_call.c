/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_margin_call.c
 *
 * PURPOSE:
 *   Exercise margin call validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/margin_call.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryMarginCall v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_margin_call_init(&v, "mc", 1000, 900, UMI_TREASURY_MARGIN_AGREED) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_margin_call_unagreed_minor(&v)!=100)return 2;
    return 0;
}
