/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_deposit_transaction.c
 *
 * PURPOSE:
 *   Exercise deposit transaction validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/deposit_transaction.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiBankingDepositTransaction v;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_banking_deposit_transaction_init(&v, "txn-1", "dep-1", 500, (UmiFinancialDate){2026,8U,25U}, (UmiFinancialDate){2026,8U,25U})!=UMI_STATUS_OK) return 1;
    /* Apply this branch only when its contract condition is satisfied. */
    if(!umi_banking_deposit_transaction_credit(&v)) return 2;
    return 0;
}
