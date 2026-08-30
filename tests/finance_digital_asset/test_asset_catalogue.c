/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_asset_catalogue.c
 *
 * PURPOSE:
 *   Implement the test asset catalogue behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/asset_catalogue.h"

int main(void)
{
    UmiDigitalAssetCatalogue catalogue;
    UmiDigitalAssetDescriptor item;
    umi_digital_asset_asset_catalogue_init(&catalogue);
    CHECK(umi_digital_asset_asset_descriptor_init(&item, "ASSET-BTC", "BTC", "Bitcoin", "BTC", 8U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_asset_catalogue_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_asset_catalogue_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_asset_catalogue_find(&catalogue, "ASSET-BTC") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
