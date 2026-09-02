/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_quant_pricing_result.c
 *
 * PURPOSE:
 *   Implement the test quant pricing result behavior for
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

#include "umicom/finance/quant/quant_pricing_result.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantQuantPricingResult value;
    CHECK(umi_quant_quant_pricing_result_init(&value, 100.0, 99.0, 1.5) == UMI_STATUS_OK);
    CHECK(umi_quant_quant_pricing_result_dirty_price(&value) > 100.49 && umi_quant_quant_pricing_result_dirty_price(&value) < 100.51);
    return 0;
}
