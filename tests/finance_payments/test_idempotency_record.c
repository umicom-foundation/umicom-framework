/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_idempotency_record.c
 *
 * PURPOSE:
 *   Exercise idempotency record validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/idempotency_record.h"
int main(void) {
    UmiPaymentsIdempotencyRecord v;
    if(umi_payments_idempotency_record_init(&v, "idem-rec", "pay-1", "idem-1", 1234U)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_idempotency_record_valid(&v)) return 2;
    return 0;
}
