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
    return 0;
}
