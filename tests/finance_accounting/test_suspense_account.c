/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_accounting/test_suspense_account.c
 *
 * PURPOSE:
 *   Exercise suspense account validation and calculations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/finance/accounting/suspense_account.h"
int main(void) {
    UmiAccountingSuspenseAccount v;
    if(umi_accounting_suspense_account_init(&v, "susp-1", "9999", 0U, 0)!=UMI_STATUS_OK) return 1;
    if(!umi_accounting_suspense_account_cleared(&v)) return 2;
    return 0;
}
