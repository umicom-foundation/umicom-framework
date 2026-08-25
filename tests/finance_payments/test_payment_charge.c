/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_charge.c
 *
 * PURPOSE:
 *   Exercise payment charge validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_charge.h"
int main(void) {
    UmiPaymentsPaymentCharge v;
    if(umi_payments_payment_charge_init(&v, "charge-1", "pay-1", 25, 5)!=UMI_STATUS_OK) return 1;
    if(umi_payments_payment_charge_total_minor(&v)!=30) return 2;
    return 0;
}
