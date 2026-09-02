/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_repair.c
 *
 * PURPOSE:
 *   Exercise payment repair validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_repair.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentRepair v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_repair_init(&v, "repair-1", "pay-1", 3U, 3U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_repair_ready(&v)) return 2;
    return 0;
}
