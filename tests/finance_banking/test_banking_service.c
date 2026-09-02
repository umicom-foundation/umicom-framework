/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_banking_service.c
 *
 * PURPOSE:
 *   Exercise banking service validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/banking_service.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingBankingService v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_banking_service_init(&v, "banking-service", true, 10U, 12U, 3U, 0U)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_banking_service_ready(&v)) return 2;
    return 0;
}
