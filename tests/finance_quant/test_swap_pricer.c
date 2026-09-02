/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_swap_pricer.c
 *
 * PURPOSE:
 *   Implement the test swap pricer behavior for
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
#include "umicom/finance/quant/swap_pricer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double pv=0.0; CHECK(umi_quant_swap_pricer_value(1000000.0,4.0,0.03,0.031,&pv)==UMI_STATUS_OK); CHECK(pv>3999.0&&pv<4001.0);
    return 0;
}
