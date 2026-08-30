/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_delivery_window.c
 *
 * PURPOSE:
 *   Implement the test delivery window behavior for
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

#include "umicom/finance/commodity/delivery_window.h"

int main(void)
{
    UmiCommodityDeliveryWindow value;
    CHECK(umi_commodity_delivery_window_init(&value, 1000, 2000, true) == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_window_valid(&value));
    return 0;
}
