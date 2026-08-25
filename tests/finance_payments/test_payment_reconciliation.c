/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_reconciliation.c
 *
 * PURPOSE:
 *   Exercise payment reconciliation validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_reconciliation.h"
int main(void) {
    UmiPaymentsPaymentReconciliation v;
    if(umi_payments_payment_reconciliation_init(&v, "recon-1", 10000, 9950, 1U)!=UMI_STATUS_OK) return 1;
    if(umi_payments_payment_reconciliation_difference_minor(&v)!=50) return 2;
    return 0;
}
