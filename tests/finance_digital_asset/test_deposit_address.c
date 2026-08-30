/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_deposit_address.c
 *
 * PURPOSE:
 *   Implement the test deposit address behavior for
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

#include "umicom/finance/digital_asset/deposit_address.h"

int main(void)
{
    UmiDigitalDepositAddress value;
    CHECK(umi_digital_asset_deposit_address_init(&value, "CUST-1", "ASSET-BTC", "BTC", "bc1qdeposit") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_deposit_address_valid(&value));
    return 0;
}
