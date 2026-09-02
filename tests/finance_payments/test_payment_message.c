/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_message.c
 *
 * PURPOSE:
 *   Exercise payment message validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_message.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentMessage v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_message_init(&v, "msg-1", "pay-1", "PAYMENT", UMI_PAYMENTS_MESSAGE_OUTBOUND, 1U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_message_valid(&v)) return 2;
    return 0;
}
