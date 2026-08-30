/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_cashflow.c
 *
 * PURPOSE:
 *   Exercise the cashflow financial-core contract.
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
#include "umicom/finance/core/cashflow.h"

int main(void)
{
    UmiCashflow c; UmiMoney m={100,2U,{{'U','S','D','\0'}}}; CHECK(umi_cashflow_init(&c,"CF","T",m,UMI_FINANCIAL_DIRECTION_RECEIVE,(UmiFinancialDate){2026,9U,1U})==UMI_STATUS_OK);
    return 0;
}
