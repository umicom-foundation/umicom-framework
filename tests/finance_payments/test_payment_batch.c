/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_batch.c
 *
 * PURPOSE:
 *   Exercise payment batch validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_batch.h"
int main(void) {
    UmiPaymentsPaymentBatch v;
    if(umi_payments_payment_batch_init(&v, "batch-1", "GBP", 3U, 3000, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_batch_releasable(&v)) return 2;
    return 0;
}
