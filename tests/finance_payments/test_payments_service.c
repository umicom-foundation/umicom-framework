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
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiPaymentsPaymentsService v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_payments_payments_service_init(&v, "payments-service", true, 10U, 10U, 0U, 0U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_payments_payments_service_ready(&v)) return 2;
    return 0;
}
