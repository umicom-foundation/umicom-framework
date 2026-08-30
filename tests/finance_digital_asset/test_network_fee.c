/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_network_fee.c
 *
 * PURPOSE:
 *   Implement the test network fee behavior for
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

#include "umicom/finance/digital_asset/network_fee.h"

int main(void)
{
    UmiDigitalNetworkFee fee;
    int64_t amount = 0;
    CHECK(umi_digital_asset_network_fee_init(&fee, "BTC", 100, 2, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_network_fee_calculate(&fee, 250U, &amount) == UMI_STATUS_OK);
    CHECK(amount == 600);
    return 0;
}
