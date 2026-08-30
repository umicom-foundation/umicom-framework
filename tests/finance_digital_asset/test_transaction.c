/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_transaction.c
 *
 * PURPOSE:
 *   Implement the test transaction behavior for
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

#include "umicom/finance/digital_asset/transaction.h"

int main(void)
{
    UmiDigitalAssetTransaction value;
    CHECK(umi_digital_asset_transaction_init(&value, "TX-1", "BTC", "from", "to", 1000, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transaction_valid(&value));
    return 0;
}
