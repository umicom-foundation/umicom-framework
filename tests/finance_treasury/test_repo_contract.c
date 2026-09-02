/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_repo_contract.c
 *
 * PURPOSE:
 *   Exercise repo contract validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/repo_contract.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiTreasuryRepoContract v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_treasury_repo_contract_init(&v, "repo", 9500, 10000, 300U) != UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(umi_treasury_repo_contract_haircut_minor(&v)!=500)return 2;
    return 0;
}
