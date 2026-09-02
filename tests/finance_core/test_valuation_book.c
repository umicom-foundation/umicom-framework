/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_valuation_book.c
 *
 * PURPOSE:
 *   Exercise the valuation book financial-core contract.
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
#include "umicom/finance/core/valuation_book.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiValuationBook b; UmiValuation x; UmiMoney m={1,2U,{{'U','S','D','\0'}}}; umi_valuation_book_init(&b); CHECK(umi_valuation_init(&x,"V",m,(UmiFinancialDate){2026,8U,25U},0U)==UMI_STATUS_OK); CHECK(umi_valuation_book_add(&b,&x)==UMI_STATUS_OK);
    return 0;
}
