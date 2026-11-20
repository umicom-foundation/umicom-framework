/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_fx_forward_pricer.c
 *
 * PURPOSE:
 *   Implement the test fx forward pricer behavior for
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
#include "umicom/finance/quant/fx_forward_pricer.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    double pv=0.0; CHECK(umi_quant_fx_forward_pricer_value(1000000.0,1.10,1.11,0.99,&pv)==UMI_STATUS_OK); CHECK(pv>9899.0&&pv<9901.0);
    static const double invalid[][4] = {
        {NAN, 1.0, 2.0, 1.0}, {1.0, NAN, 2.0, 1.0},
        {1.0, 1.0, NAN, 1.0}, {1.0, 1.0, 2.0, NAN},
        {INFINITY, 1.0, 2.0, 1.0}, {1.0, INFINITY, 2.0, 1.0},
        {1.0, 1.0, INFINITY, 1.0}, {1.0, 1.0, 2.0, INFINITY},
        {-1.0, 1.0, 2.0, 1.0}, {1.0, 0.0, 2.0, 1.0},
        {1.0, 1.0, 0.0, 1.0}, {1.0, 1.0, 2.0, -1.0}
    };
    for (size_t i = 0U; i < sizeof invalid / sizeof invalid[0]; ++i) {
        pv = 123.5;
        CHECK(umi_quant_fx_forward_pricer_value(
            invalid[i][0], invalid[i][1], invalid[i][2], invalid[i][3], &pv) == UMI_STATUS_INVALID_ARGUMENT);
        CHECK(pv == 123.5);
    }
    CHECK(umi_quant_fx_forward_pricer_value(1.0, 1.0, 2.0, 1.0, NULL) == UMI_STATUS_INVALID_ARGUMENT);
    CHECK(umi_quant_fx_forward_pricer_value(DBL_MAX, 1.0, 3.0, 1.0, &pv) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(pv == 123.5);
    CHECK(umi_quant_fx_forward_pricer_value(DBL_MAX, 3.0, 1.0, 1.0, &pv) == UMI_STATUS_CAPACITY_EXCEEDED);
    CHECK(pv == 123.5);
    CHECK(umi_quant_fx_forward_pricer_value(DBL_MAX, 1.0, 3.0, 0.25, &pv) == UMI_STATUS_OK);
    CHECK(pv == DBL_MAX / 2.0);
    CHECK(umi_quant_fx_forward_pricer_value(DBL_MAX, 3.0, 1.0, 0.0, &pv) == UMI_STATUS_OK);
    CHECK(pv == 0.0 && copysign(1.0, pv) < 0.0);
    CHECK(umi_quant_fx_forward_pricer_value(DBL_TRUE_MIN, 1.0, 1.5, 2.0, &pv) == UMI_STATUS_OK);
    CHECK(pv == DBL_TRUE_MIN);
    CHECK(umi_quant_fx_forward_pricer_value(1024.0, 1.5, 1.25, 0.5, &pv) == UMI_STATUS_OK);
    CHECK(pv == -128.0);
    CHECK(umi_quant_fx_forward_pricer_value(0.0, 1.0, 2.0, 1.0, &pv) == UMI_STATUS_OK);
    CHECK(pv == 0.0);
    return 0;
}
