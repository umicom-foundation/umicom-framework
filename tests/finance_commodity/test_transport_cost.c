#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/transport_cost.h"

int main(void)
{
    UmiCommodityTransportCost cost;
    UmiCurrency currency = {{'U','S','D','\0'}};
    int64_t amount = 0;
    CHECK(umi_commodity_transport_cost_init(&cost, "ROUTE-1", 100, 3, &currency) == UMI_STATUS_OK);
    CHECK(umi_commodity_transport_cost_calculate(&cost, 10, &amount) == UMI_STATUS_OK);
    CHECK(amount == 130);
    return 0;
}
