#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/commodity_service.h"

int main(void)
{
    UmiCommodityService service;
    UmiCommoditySnapshot snapshot;
    umi_commodity_commodity_service_init(&service);
    CHECK(umi_commodity_commodity_service_start(&service) == UMI_STATUS_OK);
    CHECK(umi_commodity_commodity_service_snapshot(&service, 1000, &snapshot) == UMI_STATUS_OK);
    CHECK(snapshot.revision == service.revision);
    return 0;
}
