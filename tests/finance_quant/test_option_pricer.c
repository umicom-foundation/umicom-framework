/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_option_pricer.c
 *
 * PURPOSE:
 *   Implement the test option pricer behavior for
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
#include "umicom/finance/quant/option_pricer.h"

int main(void)
{
    CHECK(umi_quant_option_pricer_intrinsic(105.0,100.0,UMI_QUANT_CALL)>4.9); CHECK(umi_quant_option_pricer_time_value(8.0,105.0,100.0,UMI_QUANT_CALL)>2.9);
    return 0;
}
