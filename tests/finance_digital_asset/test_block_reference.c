/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_block_reference.c
 *
 * PURPOSE:
 *   Implement the test block reference behavior for
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

#include "umicom/finance/digital_asset/block_reference.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalBlockReference value;
    CHECK(umi_digital_asset_block_reference_init(&value, "BTC", 900000U, "000000abc") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_block_reference_valid(&value));
    return 0;
}
