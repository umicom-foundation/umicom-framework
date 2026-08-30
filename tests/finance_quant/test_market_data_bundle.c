/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_market_data_bundle.c
 *
 * PURPOSE:
 *   Implement the test market data bundle behavior for
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
#include "umicom/finance/quant/market_data_bundle.h"

int main(void)
{
    UmiQuantMarketDataBundle bundle; double value=0.0;
    umi_quant_market_data_bundle_init(&bundle);
    CHECK(umi_quant_market_data_bundle_add(&bundle,"USD.OIS.1Y",0.04)==UMI_STATUS_OK);
    CHECK(umi_quant_market_data_bundle_find(&bundle,"USD.OIS.1Y",&value)==UMI_STATUS_OK);
    CHECK(value>0.039&&value<0.041);
    return 0;
}
