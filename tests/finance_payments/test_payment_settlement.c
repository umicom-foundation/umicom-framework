/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_settlement.c
 *
 * PURPOSE:
 *   Exercise payment settlement validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_settlement.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentSettlement v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_settlement_init(&v, "settle-1", "pay-1", "SET-1", 1500, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_settlement_valid(&v)) return 2;
    return 0;
}
