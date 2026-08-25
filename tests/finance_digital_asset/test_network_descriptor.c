#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/network_descriptor.h"

int main(void)
{
    UmiDigitalNetworkDescriptor value;
    CHECK(umi_digital_asset_network_descriptor_init(&value, "BTC", "Bitcoin", UMI_DIGITAL_NETWORK_UTXO, 6U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_network_descriptor_valid(&value));
    return 0;
}
