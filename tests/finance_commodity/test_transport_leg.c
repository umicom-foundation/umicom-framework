/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_transport_leg.c
 *
 * PURPOSE:
 *   Implement the test transport leg behavior for
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

#include "umicom/finance/commodity/transport_leg.h"

int main(void)
{
    UmiCommodityTransportLeg value;
    CHECK(umi_commodity_transport_leg_init(&value, "ROUTE-1", 1U, "LOC-A", "LOC-B", 1000, 2000) == UMI_STATUS_OK);
    CHECK(umi_commodity_transport_leg_valid(&value));
    return 0;
}
