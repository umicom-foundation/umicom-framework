/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_normal_model.c
 *
 * PURPOSE:
 *   Implement the test normal model behavior for
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
#include "umicom/finance/quant/normal_model.h"

int main(void)
{
    double price=0.0; CHECK(umi_quant_normal_model_price(100.0,100.0,10.0,1.0,1.0,UMI_QUANT_CALL,&price)==UMI_STATUS_OK); CHECK(price>3.98&&price<4.00);
    return 0;
}
