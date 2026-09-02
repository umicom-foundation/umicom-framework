/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_commodity/test_location.c
 *
 * PURPOSE:
 *   Implement the test location behavior for
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

#include "umicom/finance/commodity/location.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiCommodityLocation value;
    CHECK(umi_commodity_location_init(&value, "LOC-CUSHING", "Cushing", "US") == UMI_STATUS_OK);
    CHECK(umi_commodity_location_valid(&value));
    return 0;
}
