/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payment_rail_catalogue.c
 *
 * PURPOSE:
 *   Verify bounded payment rail catalogue insertion, duplicate prevention and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payment_rail_catalogue.h"
int main(void) { UmiPaymentsPaymentRailCatalogue r; UmiPaymentsPaymentRail item; umi_payments_payment_rail_catalogue_init(&r);
    if(umi_payments_payment_rail_init(&item, "rail-1", UMI_PAYMENTS_RAIL_INSTANT, "Instant Rail", 1000000, true)!=UMI_STATUS_OK) return 1;
    if(umi_payments_payment_rail_catalogue_add(&r,&item)!=UMI_STATUS_OK) return 2;
    if(umi_payments_payment_rail_catalogue_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    if(umi_payments_payment_rail_catalogue_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
