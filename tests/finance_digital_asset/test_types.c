/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_types.c
 *
 * PURPOSE:
 *   Implement the test types behavior for
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

#include "umicom/finance/digital_asset/types.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char buffer[8];
    CHECK(umi_digital_asset_copy_text(buffer, sizeof buffer, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_text_valid(buffer));
    CHECK(umi_digital_asset_copy_text(buffer, 2U, "LONG") == UMI_STATUS_CAPACITY_EXCEEDED);
    return 0;
}
