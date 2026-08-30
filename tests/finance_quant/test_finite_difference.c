/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_finite_difference.c
 *
 * PURPOSE:
 *   Implement the test finite difference behavior for
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

#include "umicom/finance/quant/finite_difference.h"

int main(void)
{
    UmiQuantFiniteDifference value;
    CHECK(umi_quant_finite_difference_init(&value, 99.0, 100.0, 101.0, 0.5) == UMI_STATUS_OK);
    CHECK(umi_quant_finite_difference_first_derivative(&value) > 1.99 && umi_quant_finite_difference_first_derivative(&value) < 2.01);
    return 0;
}
