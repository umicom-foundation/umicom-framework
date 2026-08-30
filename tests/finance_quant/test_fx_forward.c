/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_fx_forward.c
 *
 * PURPOSE:
 *   Implement the test fx forward behavior for
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

#include "umicom/finance/quant/fx_forward.h"

int main(void)
{
    UmiQuantFxForward value;
    CHECK(umi_quant_fx_forward_init(&value, 1.10, 0.02, 90) == UMI_STATUS_OK);
    CHECK(umi_quant_fx_forward_outright(&value) > 1.119 && umi_quant_fx_forward_outright(&value) < 1.121);
    return 0;
}
