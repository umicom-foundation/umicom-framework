/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_core/test_portfolio_registry.c
 *
 * PURPOSE:
 *   Exercise the portfolio registry financial-core contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#define CHECK(expr) do { if (!(expr)) return 1; } while (0)
#include <string.h>
#include "umicom/finance/core/portfolio_registry.h"

int main(void)
{
    UmiPortfolioRegistry r; UmiFinancialPortfolio x; umi_portfolio_registry_init(&r); CHECK(umi_portfolio_init(&x,"P","Portfolio","B")==UMI_STATUS_OK); CHECK(umi_portfolio_registry_add(&r,&x)==UMI_STATUS_OK);
    return 0;
}
