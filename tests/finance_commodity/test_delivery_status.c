/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_delivery_status.c
 *
 * PURPOSE:
 *   Implement the test delivery status behavior for
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

#include "umicom/finance/commodity/delivery_status.h"

int main(void)
{
    CHECK(umi_commodity_delivery_status_can_transition(UMI_COMMODITY_DELIVERY_PLANNED, UMI_COMMODITY_DELIVERY_NOMINATED));
    CHECK(!umi_commodity_delivery_status_can_transition(UMI_COMMODITY_DELIVERY_DELIVERED, UMI_COMMODITY_DELIVERY_PLANNED));
    CHECK(umi_commodity_delivery_status_terminal(UMI_COMMODITY_DELIVERY_DELIVERED));
    return 0;
}
