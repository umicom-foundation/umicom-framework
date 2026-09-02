/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_reconciliation.c
 *
 * PURPOSE:
 *   Exercise payment reconciliation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_reconciliation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentReconciliation v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_reconciliation_init(&v, "recon-1", 10000, 9950, 1U)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_payments_payment_reconciliation_difference_minor(&v)!=50) return 2;
    return 0;
}
