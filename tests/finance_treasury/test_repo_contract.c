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
int main(void) {
    UmiTreasuryRepoContract v;
    if (umi_treasury_repo_contract_init(&v, "repo", 9500, 10000, 300U) != UMI_STATUS_OK) return 1;
    if(umi_treasury_repo_contract_haircut_minor(&v)!=500)return 2;
    return 0;
}
