/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_swaption.c
 *
 * PURPOSE:
 *   Implement the test swaption behavior for
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

#include "umicom/finance/quant/swaption.h"

int main(void)
{
    UmiQuantSwaption value;
    CHECK(umi_quant_swaption_init(&value, 5000000.0, 0.03, 0.035, 1) == UMI_STATUS_OK);
    CHECK(umi_quant_swaption_intrinsic(&value) > 24999.0 && umi_quant_swaption_intrinsic(&value) < 25001.0);
    return 0;
}
