/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_payment_obligation.c
 *
 * PURPOSE:
 *   Exercise payment obligation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/payment_obligation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryPaymentObligation v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_payment_obligation_init(&v, "pay-1", 1000, 900, 100) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_payment_obligation_outstanding_minor(&v)!=800)return 2;
    return 0;
}
