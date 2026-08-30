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
int main(void) {
    UmiPaymentsPaymentValidation v;
    if(umi_payments_payment_validation_init(&v, "val-1", "pay-1", 5U, 5U, true, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_validation_passed(&v)) return 2;
    return 0;
}
