/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_account_hold.c
 *
 * PURPOSE:
 *   Exercise account hold validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/account_hold.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingAccountHold v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_account_hold_init(&v, "hold-1", "dep-1", 500, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_account_hold_valid(&v)) return 2;
    return 0;
}
