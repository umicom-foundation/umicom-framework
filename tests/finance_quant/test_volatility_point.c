/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_volatility_point.c
 *
 * PURPOSE:
 *   Implement the test volatility point behavior for
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

#include "umicom/finance/quant/volatility_point.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantVolatilityPoint value;
    CHECK(umi_quant_volatility_point_init(&value, 1.0, 100.0, 0.20) == UMI_STATUS_OK);
    CHECK(umi_quant_volatility_point_variance(&value) > 0.039 && umi_quant_volatility_point_variance(&value) < 0.041);
    return 0;
}
