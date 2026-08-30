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
int main(void) {
    UmiPaymentsPaymentRail v;
    if(umi_payments_payment_rail_init(&v, "rail-1", UMI_PAYMENTS_RAIL_INSTANT, "Instant Rail", 1000000, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_rail_valid(&v)) return 2;
    return 0;
}
