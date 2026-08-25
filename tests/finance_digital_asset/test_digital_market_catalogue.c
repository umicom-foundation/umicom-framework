#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/digital_market_catalogue.h"

int main(void)
{
    UmiDigitalMarketCatalogue catalogue;
    UmiDigitalMarket item;
    umi_digital_asset_digital_market_catalogue_init(&catalogue);
    CHECK(umi_digital_asset_digital_market_init(&item, "BTC-USD", "ASSET-BTC", "ASSET-USD", "UMICOM-X", 1) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_digital_market_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_digital_market_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_digital_market_catalogue_find(&catalogue, "BTC-USD") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
