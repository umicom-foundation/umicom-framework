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
int main(void) {
    UmiPaymentsPaymentReturn v;
    if(umi_payments_payment_return_init(&v, "return-1", "pay-1", "AC04", 1500)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_return_valid(&v)) return 2;
    return 0;
}
