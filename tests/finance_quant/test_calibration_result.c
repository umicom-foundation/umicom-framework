/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_calibration_result.c
 *
 * PURPOSE:
 *   Implement the test calibration result behavior for
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

#include "umicom/finance/quant/calibration_result.h"

int main(void)
{
    UmiQuantCalibrationResult value;
    CHECK(umi_quant_calibration_result_init(&value, 0.20, 0.19, 2.0) == UMI_STATUS_OK);
    CHECK(umi_quant_calibration_result_weighted_error(&value) > 0.019 && umi_quant_calibration_result_weighted_error(&value) < 0.021);
    return 0;
}
