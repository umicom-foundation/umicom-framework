#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/market_data_cache.h"

int main(void)
{
    UmiEnterpriseMarketDataCache c; UmiEnterpriseMarketDataUpdate u;
    umi_enterprise_market_data_cache_init(&c);
    CHECK(umi_enterprise_market_data_update_init(&u, "X", 10.0, 1U, 1) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_cache_put(&c, &u) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_cache_find(&c, "X") != NULL);
    return 0;
}
