/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_digital_asset_service.c
 *
 * PURPOSE:
 *   Implement the test digital asset service behavior for
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

#include "umicom/finance/digital_asset/digital_asset_service.h"

int main(void)
{
    UmiDigitalAssetService service;
    umi_digital_asset_digital_asset_service_init(&service);
    CHECK(!umi_digital_asset_digital_asset_service_ready(&service));
    CHECK(umi_digital_asset_digital_asset_service_start(&service) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_digital_asset_service_ready(&service));
    return 0;
}
