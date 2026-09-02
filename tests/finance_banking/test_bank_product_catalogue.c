/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_bank_product_catalogue.c
 *
 * PURPOSE:
 *   Verify bounded bank product catalogue insertion, duplicate prevention and lookup.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product_catalogue.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) { UmiBankingBankProductCatalogue r; UmiBankingBankProduct item; umi_banking_bank_product_catalogue_init(&r);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_product_init(&item, "prod-1", "Current Account", UMI_BANKING_PRODUCT_DEPOSIT, true)!=UMI_STATUS_OK) return 1;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_product_catalogue_add(&r,&item)!=UMI_STATUS_OK) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_product_catalogue_add(&r,&item)!=UMI_STATUS_ALREADY_EXISTS) return 3;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(umi_banking_bank_product_catalogue_find(&r,item.id.value)==NULL) return 4;
    return 0;
}
