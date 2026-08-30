/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_discount_curve.c
 *
 * PURPOSE:
 *   Implement the test discount curve behavior for
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
#include "umicom/finance/quant/discount_curve.h"

int main(void)
{
    double factor = 0.0, pv = 0.0;
    CHECK(umi_quant_discount_curve_factor(0.05, 365, &factor) == UMI_STATUS_OK);
    CHECK(factor > 0.951 && factor < 0.952);
    CHECK(umi_quant_discount_curve_present_value(100.0, 0.05, 365, &pv) == UMI_STATUS_OK);
    CHECK(pv > 95.1 && pv < 95.2);
    return 0;
}
