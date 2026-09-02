/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_sensitivity_result.c
 *
 * PURPOSE:
 *   Implement the test sensitivity result behavior for
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

#include "umicom/finance/quant/sensitivity_result.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantSensitivityResult value;
    CHECK(umi_quant_sensitivity_result_init(&value, 98.0, 100.0, 102.0, 1.0) == UMI_STATUS_OK);
    CHECK(umi_quant_sensitivity_result_delta(&value) > 1.99 && umi_quant_sensitivity_result_delta(&value) < 2.01);
    return 0;
}
