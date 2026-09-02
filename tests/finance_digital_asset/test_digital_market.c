/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_digital_market.c
 *
 * PURPOSE:
 *   Implement the test digital market behavior for
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

#include "umicom/finance/digital_asset/digital_market.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalMarket value;
    CHECK(umi_digital_asset_digital_market_init(&value, "BTC-USD", "ASSET-BTC", "ASSET-USD", "UMICOM-X", 1) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_digital_market_valid(&value));
    return 0;
}
