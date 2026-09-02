/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_subscription.c
 *
 * PURPOSE:
 *   Implement the test market data subscription behavior for
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

#include "umicom/finance/enterprise/market_data_subscription.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEnterpriseMarketDataSubscription s;
    CHECK(umi_enterprise_market_data_subscription_init(&s, "sub-1", "EURUSD.SPOT") == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_subscription_set_active(&s, 1) == UMI_STATUS_OK);
    CHECK(s.active == 1);
    return 0;
}
