/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_validation.c
 *
 * PURPOSE:
 *   Exercise payment validation validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_validation.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentValidation v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_validation_init(&v, "val-1", "pay-1", 5U, 5U, true, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_validation_passed(&v)) return 2;
    return 0;
}
