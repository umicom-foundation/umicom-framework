/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_deposit_transaction.c
 *
 * PURPOSE:
 *   Exercise deposit transaction validation and calculations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_transaction.h"
int main(void) {
    UmiBankingDepositTransaction v;
    if(umi_banking_deposit_transaction_init(&v, "txn-1", "dep-1", 500, (UmiFinancialDate){2026,8U,25U}, (UmiFinancialDate){2026,8U,25U})!=UMI_STATUS_OK) return 1;
    if(!umi_banking_deposit_transaction_credit(&v)) return 2;
    return 0;
}
