/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_deposit_account.c
 *
 * PURPOSE:
 *   Exercise deposit account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account.h"
int main(void) {
    UmiBankingDepositAccount v;
    if(umi_banking_deposit_account_init(&v, "dep-1", "cust-1", "prod-1", "GBP", 10000, 8500, UMI_BANKING_ACCOUNT_OPEN)!=UMI_STATUS_OK) return 1;
    if(umi_banking_deposit_account_reserved_minor(&v)!=1500) return 2;
    return 0;
}
