/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_route.c
 *
 * PURPOSE:
 *   Exercise payment route validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_route.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentRoute v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payment_route_init(&v, "route-1", UMI_PAYMENTS_RAIL_HIGH_VALUE, 1U, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payment_route_valid(&v)) return 2;
    return 0;
}
