/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_volatility_surface.c
 *
 * PURPOSE:
 *   Implement the test volatility surface behavior for
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
#include "umicom/finance/quant/volatility_surface.h"

int main(void)
{
    UmiQuantVolatilitySurface surface; double vol=0.0;
    umi_quant_volatility_surface_init(&surface);
    CHECK(umi_quant_volatility_surface_add(&surface,1.0,100.0,0.20)==UMI_STATUS_OK);
    CHECK(umi_quant_volatility_surface_add(&surface,2.0,110.0,0.25)==UMI_STATUS_OK);
    CHECK(umi_quant_volatility_surface_nearest(&surface,1.1,101.0,&vol)==UMI_STATUS_OK);
    CHECK(vol>0.199 && vol<0.201);
    return 0;
}
