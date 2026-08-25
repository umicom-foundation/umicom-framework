#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/network_catalogue.h"

int main(void)
{
    UmiDigitalNetworkCatalogue catalogue;
    UmiDigitalNetworkDescriptor item;
    umi_digital_asset_network_catalogue_init(&catalogue);
    CHECK(umi_digital_asset_network_descriptor_init(&item, "BTC", "Bitcoin", UMI_DIGITAL_NETWORK_UTXO, 6U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_network_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_network_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_network_catalogue_find(&catalogue, "BTC") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
