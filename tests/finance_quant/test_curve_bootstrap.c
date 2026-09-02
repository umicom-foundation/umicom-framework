/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_curve_bootstrap.c
 *
 * PURPOSE:
 *   Implement the test curve bootstrap behavior for
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
#include "umicom/finance/quant/curve_bootstrap.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double df = 0.0;
    CHECK(umi_quant_curve_bootstrap_discount_factor(0.05, 0.5, 0.48, &df) == UMI_STATUS_OK);
    CHECK(df > 0.951 && df < 0.953);
    return 0;
}
