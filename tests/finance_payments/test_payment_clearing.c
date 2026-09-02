/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_clearing.c
 *
 * PURPOSE:
 *   Exercise payment clearing validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_clearing.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentClearing v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_clearing_init(&v, "clear-1", 10000, -2000, 4U, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_clearing_valid(&v)) return 2;
    return 0;
}
