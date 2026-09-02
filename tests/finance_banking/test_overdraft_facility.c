/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_overdraft_facility.c
 *
 * PURPOSE:
 *   Exercise overdraft facility validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/overdraft_facility.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingOverdraftFacility v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_overdraft_facility_init(&v, "od-1", "dep-1", 10000, 2500)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_banking_overdraft_facility_available_minor(&v)!=7500) return 2;
    return 0;
}
