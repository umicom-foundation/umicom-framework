/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_account_registry.c
 *
 * PURPOSE:
 *   Exercise the account registry financial-core contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/account_registry.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAccountRegistry r; UmiFinancialCoreAccount x; umi_account_registry_init(&r); CHECK(umi_financial_account_init(&x,"A","Account","LE","CASH")==UMI_STATUS_OK); CHECK(umi_account_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
