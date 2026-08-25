#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/asset_descriptor.h"

int main(void)
{
    UmiDigitalAssetDescriptor value;
    CHECK(umi_digital_asset_asset_descriptor_init(&value, "ASSET-BTC", "BTC", "Bitcoin", "BTC", 8U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_asset_descriptor_valid(&value));
    return 0;
}
