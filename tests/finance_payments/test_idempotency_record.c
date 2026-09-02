/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_idempotency_record.c
 *
 * PURPOSE:
 *   Exercise idempotency record validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/idempotency_record.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsIdempotencyRecord v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_idempotency_record_init(&v, "idem-rec", "pay-1", "idem-1", 1234U)!=UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if(!umi_payments_idempotency_record_valid(&v)) return 2;
    return 0;
}
