/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_treasury/test_bank_account_registry.c
 *
 * PURPOSE:
 *   Verify bounded bank account registry registration and lookup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/treasury/bank_account_registry.h"
int main(void) { UmiTreasuryBankAccountRegistry r; UmiTreasuryBankAccount item; umi_treasury_bank_account_registry_init(&r);
    if (umi_treasury_bank_account_init(&item, "ba-1", "bank-1", "USD", true) != UMI_STATUS_OK) return 1;
    if (umi_treasury_bank_account_registry_add(&r,&item) != UMI_STATUS_OK) return 2;
    if (umi_treasury_bank_account_registry_find(&r,item.id) == NULL) return 3;
    if (umi_treasury_bank_account_registry_add(&r,&item) != UMI_STATUS_ALREADY_EXISTS) return 4;
    return 0; }
