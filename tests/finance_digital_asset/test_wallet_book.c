/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_wallet_book.c
 *
 * PURPOSE:
 *   Implement the test wallet book behavior for
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

#include "umicom/finance/digital_asset/wallet_book.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalAssetWalletBook catalogue;
    UmiDigitalAssetWallet item;
    umi_digital_asset_wallet_book_init(&catalogue);
    CHECK(umi_digital_asset_wallet_init(&item, "WALLET-1", "Operations", "BTC", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_wallet_book_add(&catalogue, &item) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_wallet_book_add(&catalogue, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_digital_asset_wallet_book_find(&catalogue, "WALLET-1") != NULL);
    CHECK(catalogue.revision == 1U);
    
    return 0;
}
