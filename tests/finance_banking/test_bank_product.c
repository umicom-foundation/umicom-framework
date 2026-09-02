/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_bank_product.c
 *
 * PURPOSE:
 *   Exercise bank product validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingBankProduct v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_bank_product_init(&v, "prod-1", "Current Account", UMI_BANKING_PRODUCT_DEPOSIT, true)!=UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if(!umi_banking_bank_product_valid(&v)) return 2;
    return 0;
}
