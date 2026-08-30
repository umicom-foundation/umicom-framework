/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_topic.c
 *
 * PURPOSE:
 *   Implement the test market data topic behavior for
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

#include "umicom/finance/enterprise/market_data_topic.h"

int main(void)
{
    UmiEnterpriseMarketDataTopic t;
    CHECK(umi_enterprise_market_data_topic_init(&t, "EURUSD.MID", "EURUSD", "mid") == UMI_STATUS_OK);
    CHECK(t.field[0] == 'm');
    return 0;
}
