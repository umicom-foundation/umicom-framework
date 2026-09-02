/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_cache.c
 *
 * PURPOSE:
 *   Implement the test market data cache behavior for
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

#include "umicom/finance/enterprise/market_data_cache.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEnterpriseMarketDataCache c; UmiEnterpriseMarketDataUpdate u;
    umi_enterprise_market_data_cache_init(&c);
    CHECK(umi_enterprise_market_data_update_init(&u, "X", 10.0, 1U, 1) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_cache_put(&c, &u) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_cache_find(&c, "X") != NULL);
    return 0;
}
