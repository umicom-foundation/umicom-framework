/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_deposit_account_book.c
 *
 * PURPOSE:
 *   Verify bounded deposit account book insertion, duplicate prevention and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_account_book.h"
int main(void) { UmiBankingDepositAccountBook r; UmiBankingDepositAccount item; umi_banking_deposit_account_book_init(&r);
    if(umi_banking_deposit_account_init(&item, "dep-1", "cust-1", "prod-1", "GBP", 10000, 8500, UMI_BANKING_ACCOUNT_OPEN)!=UMI_STATUS_OK) return 1;
    if(umi_banking_deposit_account_book_add(&r,&item)!=UMI_STATUS_OK) return 2;
    if(umi_banking_deposit_account_book_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    if(umi_banking_deposit_account_book_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
