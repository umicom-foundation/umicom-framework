/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_greek_set.c
 *
 * PURPOSE:
 *   Implement the test greek set behavior for
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

#include "umicom/finance/quant/greek_set.h"

int main(void)
{
    UmiQuantGreekSet value;
    CHECK(umi_quant_greek_set_init(&value, 0.5, 0.02, 0.1, -0.03, 0.04) == UMI_STATUS_OK);
    CHECK(umi_quant_greek_set_absolute_first_order(&value) > 0.66 && umi_quant_greek_set_absolute_first_order(&value) < 0.68);
    return 0;
}
