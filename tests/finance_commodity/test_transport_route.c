/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_transport_route.c
 *
 * PURPOSE:
 *   Implement the test transport route behavior for
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

#include "umicom/finance/commodity/transport_route.h"

int main(void)
{
    UmiCommodityTransportRoute value;
    CHECK(umi_commodity_transport_route_init(&value, "ROUTE-1", "LOC-A", "LOC-B", "VESSEL") == UMI_STATUS_OK);
    CHECK(umi_commodity_transport_route_valid(&value));
    return 0;
}
