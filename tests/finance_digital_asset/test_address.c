/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_address.c
 *
 * PURPOSE:
 *   Implement the test address behavior for
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

#include "umicom/finance/digital_asset/address.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalAssetAddress value;
    CHECK(umi_digital_asset_address_init(&value, "BTC", "bc1qexample", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_address_valid(&value));
    CHECK(value.verified);
    return 0;
}
