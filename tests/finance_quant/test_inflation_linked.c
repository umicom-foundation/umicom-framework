/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_inflation_linked.c
 *
 * PURPOSE:
 *   Implement the test inflation linked behavior for
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

#include "umicom/finance/quant/inflation_linked.h"

int main(void)
{
    UmiQuantInflationLinked value;
    CHECK(umi_quant_inflation_linked_init(&value, 100.0, 110.0, 1000.0) == UMI_STATUS_OK);
    CHECK(umi_quant_inflation_linked_adjusted_principal(&value) > 1099.9 && umi_quant_inflation_linked_adjusted_principal(&value) < 1100.1);
    return 0;
}
