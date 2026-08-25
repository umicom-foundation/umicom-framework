#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/market_data_freshness.h"

int main(void){ UmiEnterpriseMarketDataFreshnessPolicy p; CHECK(umi_enterprise_market_data_freshness_policy_init(&p,100)==UMI_STATUS_OK); CHECK(umi_enterprise_market_data_is_fresh(&p,950,1000)==1); CHECK(umi_enterprise_market_data_is_fresh(&p,800,1000)==0); return 0; }
