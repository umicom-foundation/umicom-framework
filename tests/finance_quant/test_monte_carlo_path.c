/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_monte_carlo_path.c
 *
 * PURPOSE:
 *   Implement the test monte carlo path behavior for
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
#include "umicom/finance/quant/monte_carlo_path.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double terminal=0.0; CHECK(umi_quant_monte_carlo_path_terminal(100.0,0.05,0.0,1.0,0.0,&terminal)==UMI_STATUS_OK); CHECK(terminal>105.1&&terminal<105.2);
    return 0;
}
