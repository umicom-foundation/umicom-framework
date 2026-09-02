/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_valuation.c
 *
 * PURPOSE:
 *   Exercise the valuation financial-core contract.
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
#include "umicom/finance/core/valuation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiValuation x; UmiMoney m={10,2U,{{'U','S','D','\0'}}}; CHECK(umi_valuation_init(&x,"ID",m,(UmiFinancialDate){2026,8U,25U},1U)==UMI_STATUS_OK); CHECK(umi_valuation_is_valid(&x));
    return 0;
}
