#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/commodity/location_catalogue.h"

int main(void)
{
    UmiCommodityLocationCatalogue catalogue;
    UmiCommodityLocation item;
    umi_commodity_location_catalogue_init(&catalogue);
    CHECK(umi_commodity_location_init(&item, "LOC-1", "Terminal One", "GB") == UMI_STATUS_OK);
    CHECK(umi_commodity_location_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_commodity_location_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_commodity_location_catalogue_find(&catalogue, "LOC-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
