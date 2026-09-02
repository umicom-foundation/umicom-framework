/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_banking_snapshot.c
 *
 * PURPOSE:
 *   Exercise banking snapshot validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/banking_snapshot.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingBankingSnapshot v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_banking_snapshot_init(&v, "snap-1", 10U, 8U, 3U, 1000000, 600000)!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_banking_banking_snapshot_net_funding_minor(&v)!=400000) return 2;
    return 0;
}
