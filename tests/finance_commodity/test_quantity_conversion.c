/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_quantity_conversion.c
 *
 * PURPOSE:
 *   Implement the test quantity conversion behavior for
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

#include "umicom/finance/commodity/quantity_conversion.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityQuantityConversion conversion;
    int64_t output = 0;
    CHECK(umi_commodity_quantity_conversion_init(&conversion, "MT", "KG", 1000, 1) == UMI_STATUS_OK);
    CHECK(umi_commodity_quantity_conversion_apply(&conversion, 3, &output) == UMI_STATUS_OK);
    CHECK(output == 3000);
    return 0;
}
