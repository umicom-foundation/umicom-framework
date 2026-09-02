/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_rail.c
 *
 * PURPOSE:
 *   Exercise payment rail validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_rail.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentRail v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_rail_init(&v, "rail-1", UMI_PAYMENTS_RAIL_INSTANT, "Instant Rail", 1000000, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_rail_valid(&v)) return 2;
    return 0;
}
