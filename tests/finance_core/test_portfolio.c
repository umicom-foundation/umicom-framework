/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_portfolio.c
 *
 * PURPOSE:
 *   Exercise the portfolio financial-core contract.
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
#include "umicom/finance/core/portfolio.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiFinancialPortfolio x; CHECK(umi_portfolio_init(&x,"ID","Name","PARENT")==UMI_STATUS_OK); CHECK(umi_portfolio_is_valid(&x));
    return 0;
}
