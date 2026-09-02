/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_return.c
 *
 * PURPOSE:
 *   Exercise payment return validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_return.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentReturn v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_return_init(&v, "return-1", "pay-1", "AC04", 1500)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_return_valid(&v)) return 2;
    return 0;
}
