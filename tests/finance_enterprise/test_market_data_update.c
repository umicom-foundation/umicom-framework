/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_update.c
 *
 * PURPOSE:
 *   Implement the test market data update behavior for
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

#include "umicom/finance/enterprise/market_data_update.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEnterpriseMarketDataUpdate u;
    CHECK(umi_enterprise_market_data_update_init(&u, "GBPUSD.MID", 1.31, 7U, 1000) == UMI_STATUS_OK);
    CHECK(u.sequence == 7U);
    return 0;
}
