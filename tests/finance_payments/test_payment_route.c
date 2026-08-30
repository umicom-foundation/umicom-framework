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
int main(void) {
    UmiPaymentsPaymentRoute v;
    if(umi_payments_payment_route_init(&v, "route-1", UMI_PAYMENTS_RAIL_HIGH_VALUE, 1U, true)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payment_route_valid(&v)) return 2;
    return 0;
}
