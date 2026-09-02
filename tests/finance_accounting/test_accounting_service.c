/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_accounting_service.c
 *
 * PURPOSE:
 *   Exercise accounting service validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/accounting_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAccountingAccountingService v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_accounting_accounting_service_init(&v, "accounting-service", true, 20U, 100U, 1U, 0U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_accounting_accounting_service_ready(&v)) return 2;
    return 0;
}
