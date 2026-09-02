/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_charge.c
 *
 * PURPOSE:
 *   Exercise payment charge validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_charge.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentCharge v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_charge_init(&v, "charge-1", "pay-1", 25, 5)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_payments_payment_charge_total_minor(&v)!=30) return 2;
    return 0;
}
