/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_fee_quote.c
 *
 * PURPOSE:
 *   Implement the test fee quote behavior for
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

#include "umicom/finance/digital_asset/fee_quote.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalFeeQuote value;
    CHECK(umi_digital_asset_fee_quote_init(&value, "BTC", 1200, 8, "BTC", 5000) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_fee_quote_valid(&value));
    return 0;
}
