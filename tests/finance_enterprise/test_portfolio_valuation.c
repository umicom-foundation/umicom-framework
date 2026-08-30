/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_portfolio_valuation.c
 *
 * PURPOSE:
 *   Implement the test portfolio valuation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/portfolio_valuation.h"

int main(void){ UmiEnterprisePortfolioValuation v; CHECK(umi_enterprise_portfolio_valuation_init(&v,"p",1,105.0,100.0)==UMI_STATUS_OK); CHECK(v.pnl==5.0); return 0; }
