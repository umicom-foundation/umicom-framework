/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_curve_interpolation.c
 *
 * PURPOSE:
 *   Implement the test curve interpolation behavior for
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
#include "umicom/finance/quant/curve_interpolation.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double value = 0.0;
    CHECK(umi_quant_curve_interpolation_linear(0.0, 1.0, 10.0, 3.0, 5.0, &value) == UMI_STATUS_OK);
    CHECK(value > 1.99 && value < 2.01);
    CHECK(umi_quant_curve_interpolation_log_linear(0.0, 1.0, 2.0, 4.0, 1.0, &value) == UMI_STATUS_OK);
    CHECK(value > 1.99 && value < 2.01);
    return 0;
}
