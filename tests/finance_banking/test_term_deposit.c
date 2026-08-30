/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_banking/test_term_deposit.c
 *
 * PURPOSE:
 *   Exercise term deposit validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/banking/term_deposit.h"
int main(void) {
    UmiBankingTermDeposit v;
    if(umi_banking_term_deposit_init(&v, "td-1", "cust-1", 100000, (UmiFinancialDate){2026,1U,1U}, (UmiFinancialDate){2027,1U,1U}, true)!=UMI_STATUS_OK) return 1;
    if(!umi_banking_term_deposit_valid(&v)) return 2;
    return 0;
}
