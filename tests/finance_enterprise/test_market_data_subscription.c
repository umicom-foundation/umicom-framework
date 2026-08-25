#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/market_data_subscription.h"

int main(void)
{
    UmiEnterpriseMarketDataSubscription s;
    CHECK(umi_enterprise_market_data_subscription_init(&s, "sub-1", "EURUSD.SPOT") == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_subscription_set_active(&s, 1) == UMI_STATUS_OK);
    CHECK(s.active == 1);
    return 0;
}
