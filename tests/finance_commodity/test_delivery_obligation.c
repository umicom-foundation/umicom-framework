#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/delivery_obligation.h"

int main(void)
{
    UmiCommodityDeliveryObligation value;
    CHECK(umi_commodity_delivery_obligation_init(&value, "DEL-1", "CTR-1", 100, 0, "MT", 5000) == UMI_STATUS_OK);
    CHECK(umi_commodity_delivery_obligation_valid(&value));
    return 0;
}
