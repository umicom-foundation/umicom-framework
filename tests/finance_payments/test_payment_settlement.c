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
int main(void) {
    UmiPaymentsPaymentSettlement v;
    if(umi_payments_payment_settlement_init(&v, "settle-1", "pay-1", "SET-1", 1500, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_settlement_valid(&v)) return 2;
    return 0;
}
