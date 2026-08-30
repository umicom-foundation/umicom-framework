/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_bank_account.c
 *
 * PURPOSE:
 *   Exercise bank account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/bank_account.h"
int main(void) {
    UmiTreasuryBankAccount v;
    if (umi_treasury_bank_account_init(&v, "ba-1", "bank-1", "GBP", true) != UMI_STATUS_OK) return 1;
    if(!umi_treasury_bank_account_usable(&v))return 2;
    return 0;
}
