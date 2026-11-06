/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_discounting_engine.c
 *
 * PURPOSE:
 *   Implement the test discounting engine behavior for
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

#include <math.h>
#include <float.h>
#include "umicom/finance/quant/discounting_engine.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double amounts[2]={100.0,100.0}; double rates[2]={0.0,0.0}; int32_t days[2]={30,60}; double pv=0.0;
    CHECK(umi_quant_discounting_engine_portfolio(amounts,rates,days,2U,&pv)==UMI_STATUS_OK);
    CHECK(pv>199.9&&pv<200.1);
    pv = 42.0;
    CHECK(umi_quant_discounting_engine_cashflow(INFINITY, 0.0, 365, &pv) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_quant_discounting_engine_cashflow(100.0, NAN, 365, &pv) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_quant_discounting_engine_cashflow(DBL_MAX, -1.0, 365, &pv) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(pv == 42.0);
    amounts[0] = amounts[1] = DBL_MAX;
    CHECK(umi_quant_discounting_engine_portfolio(amounts, rates, days, 2U, &pv) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(pv == 42.0);
    amounts[1] = NAN;
    CHECK(umi_quant_discounting_engine_portfolio(amounts, rates, days, 2U, &pv) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(pv == 42.0);
    amounts[1] = -DBL_MAX;
    CHECK(umi_quant_discounting_engine_portfolio(amounts, rates, days, 2U, &pv) == UMI_STATUS_OK && pv == 0.0);
    CHECK(umi_quant_discounting_engine_portfolio(amounts, rates, days, 0U, &pv) == UMI_STATUS_OK && pv == 0.0);
    return 0;
}
