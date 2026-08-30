/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_payments/test_payments_service.c
 *
 * PURPOSE:
 *   Exercise payments service validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/payments/payments_service.h"
int main(void) {
    UmiPaymentsPaymentsService v;
    if(umi_payments_payments_service_init(&v, "payments-service", true, 10U, 10U, 0U, 0U)!=UMI_STATUS_OK) return 1;
    if(!umi_payments_payments_service_ready(&v)) return 2;
    return 0;
}
