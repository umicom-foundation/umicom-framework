/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_batch.c
 *
 * PURPOSE:
 *   Exercise payment batch validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_batch.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentBatch v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_batch_init(&v, "batch-1", "GBP", 3U, 3000, true)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_payments_payment_batch_releasable(&v)) return 2;
    return 0;
}
