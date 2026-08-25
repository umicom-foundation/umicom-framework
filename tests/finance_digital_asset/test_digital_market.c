#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/digital_market.h"

int main(void)
{
    UmiDigitalMarket value;
    CHECK(umi_digital_asset_digital_market_init(&value, "BTC-USD", "ASSET-BTC", "ASSET-USD", "UMICOM-X", 1) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_digital_market_valid(&value));
    return 0;
}
