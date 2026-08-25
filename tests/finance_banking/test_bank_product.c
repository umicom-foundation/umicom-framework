/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_bank_product.c
 *
 * PURPOSE:
 *   Exercise bank product validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/bank_product.h"
int main(void) {
    UmiBankingBankProduct v;
    if(umi_banking_bank_product_init(&v, "prod-1", "Current Account", UMI_BANKING_PRODUCT_DEPOSIT, true)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_bank_product_valid(&v)) return 2;
    return 0;
}
