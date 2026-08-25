/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_balance.c
 *
 * PURPOSE:
 *   Exercise the balance financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/balance.h"

int main(void)
{
    UmiFinancialBalance x; UmiMoney m={10,2U,{{'U','S','D','\0'}}}; CHECK(umi_balance_init(&x,"ID",m,(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_balance_is_valid(&x));
    return 0;
}
