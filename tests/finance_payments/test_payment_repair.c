/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_repair.c
 *
 * PURPOSE:
 *   Exercise payment repair validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_repair.h"
int main(void) {
    UmiPaymentsPaymentRepair v;
    if(umi_payments_payment_repair_init(&v, "repair-1", "pay-1", 3U, 3U)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_repair_ready(&v)) return 2;
    return 0;
}
