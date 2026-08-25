#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/address.h"

int main(void)
{
    UmiDigitalAssetAddress value;
    CHECK(umi_digital_asset_address_init(&value, "BTC", "bc1qexample", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_address_valid(&value));
    CHECK(value.verified);
    return 0;
}
