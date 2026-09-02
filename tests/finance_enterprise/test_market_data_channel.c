/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_channel.c
 *
 * PURPOSE:
 *   Implement the test market data channel behavior for
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

#include "umicom/finance/enterprise/market_data_channel.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiEnterpriseMarketDataChannel c;
    CHECK(umi_enterprise_market_data_channel_init(&c, "primary") == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_channel_accept(&c, 1U, 1000) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_channel_accept(&c, 1U, 1001) == UMI_STATUS_INVALID_STATE);
    return 0;
}
