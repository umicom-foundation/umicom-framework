/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_pricing_engine.c
 *
 * PURPOSE:
 *   Implement the test pricing engine behavior for
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
#include "umicom/finance/quant/pricing_engine.h"

int main(void)
{
    double pv=0.0; CHECK(umi_quant_pricing_engine_value(UMI_QUANT_PRODUCT_FORWARD,100.0,52.0,50.0,0.99,&pv)==UMI_STATUS_OK); CHECK(pv>197.9&&pv<198.1);
    return 0;
}
