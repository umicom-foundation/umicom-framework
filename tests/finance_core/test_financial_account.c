/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_financial_account.c
 *
 * PURPOSE:
 *   Exercise the financial account financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/financial_account.h"

int main(void)
{
    UmiFinancialCoreAccount x; CHECK(umi_financial_account_init(&x,"ID","Name","PARENT","CODE")==UMI_STATUS_OK); CHECK(umi_financial_account_is_valid(&x));
    return 0;
}
