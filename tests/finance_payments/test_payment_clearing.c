/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_clearing.c
 *
 * PURPOSE:
 *   Exercise payment clearing validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_clearing.h"
int main(void) {
    UmiPaymentsPaymentClearing v;
    if(umi_payments_payment_clearing_init(&v, "clear-1", 10000, -2000, 4U, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_clearing_valid(&v)) return 2;
    return 0;
}
