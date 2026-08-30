/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_wallet.c
 *
 * PURPOSE:
 *   Implement the test wallet behavior for
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

#include "umicom/finance/digital_asset/wallet.h"

int main(void)
{
    UmiDigitalAssetWallet value;
    CHECK(umi_digital_asset_wallet_init(&value, "WALLET-1", "Operations", "BTC", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_wallet_valid(&value));
    return 0;
}
