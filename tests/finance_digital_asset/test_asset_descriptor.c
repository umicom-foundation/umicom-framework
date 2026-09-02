/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_asset_descriptor.c
 *
 * PURPOSE:
 *   Implement the test asset descriptor behavior for
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

#include "umicom/finance/digital_asset/asset_descriptor.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalAssetDescriptor value;
    CHECK(umi_digital_asset_asset_descriptor_init(&value, "ASSET-BTC", "BTC", "Bitcoin", "BTC", 8U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_asset_descriptor_valid(&value));
    return 0;
}
