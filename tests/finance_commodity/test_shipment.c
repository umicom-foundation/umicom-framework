#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/shipment.h"

int main(void)
{
    UmiCommodityShipment value;
    CHECK(umi_commodity_shipment_init(&value, "SHIP-1", "CTR-1", "ROUTE-1", 500, 0, "MT") == UMI_STATUS_OK);
    CHECK(umi_commodity_shipment_valid(&value));
    return 0;
}
