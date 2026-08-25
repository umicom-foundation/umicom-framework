/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_balance_book.c
 *
 * PURPOSE:
 *   Exercise the balance book financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/balance_book.h"

int main(void)
{
    UmiBalanceBook b; UmiFinancialBalance x; UmiMoney m={1,2U,{{'U','S','D','\0'}}}; umi_balance_book_init(&b); CHECK(umi_balance_init(&x,"B",m,(UmiFinancialDate){2026,8U,25U},0U)==UMI_STATUS_OK); CHECK(umi_balance_book_add(&b,&x)==UMI_STATUS_OK);
    return 0;
}
