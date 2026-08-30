/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_shipment_book.c
 *
 * PURPOSE:
 *   Implement the test shipment book behavior for
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

#include "umicom/finance/commodity/shipment_book.h"

int main(void)
{
    UmiCommodityShipmentBook catalogue;
    UmiCommodityShipment item;
    umi_commodity_shipment_book_init(&catalogue);
    CHECK(umi_commodity_shipment_init(&item, "SHIP-1", "CTR-1", "ROUTE-1", 500, 0, "MT") == UMI_STATUS_OK);
    CHECK(umi_commodity_shipment_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_shipment_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_shipment_book_find(&catalogue, "SHIP-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
