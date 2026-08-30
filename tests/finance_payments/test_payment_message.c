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
int main(void) {
    UmiPaymentsPaymentMessage v;
    if(umi_payments_payment_message_init(&v, "msg-1", "pay-1", "PAYMENT", UMI_PAYMENTS_MESSAGE_OUTBOUND, 1U)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_message_valid(&v)) return 2;
    return 0;
}
