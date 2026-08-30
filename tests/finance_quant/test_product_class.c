/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_product_class.c
 *
 * PURPOSE:
 *   Implement the test product class behavior for
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
#include "umicom/finance/quant/product_class.h"

int main(void)
{
    CHECK(umi_quant_product_class_is_derivative(UMI_QUANT_PRODUCT_OPTION));
    CHECK(!umi_quant_product_class_is_derivative(UMI_QUANT_PRODUCT_BOND));
    CHECK(umi_quant_product_class_name(UMI_QUANT_PRODUCT_SWAP)[0] == 's');
    return 0;
}
