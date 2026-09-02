/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_nostro_account.c
 *
 * PURPOSE:
 *   Exercise nostro account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/nostro_account.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryNostroAccount v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_nostro_account_init(&v, "nostro", 1000, 300) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_nostro_account_available_minor(&v)!=700)return 2;
    return 0;
}
