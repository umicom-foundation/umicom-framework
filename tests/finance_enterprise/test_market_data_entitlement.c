#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/market_data_entitlement.h"

int main(void){ UmiEnterpriseMarketDataEntitlement e; CHECK(umi_enterprise_market_data_entitlement_init(&e,"risk","EUR",1)==UMI_STATUS_OK); CHECK(umi_enterprise_market_data_entitlement_allows(&e,"EURUSD.MID")==1); CHECK(umi_enterprise_market_data_entitlement_allows(&e,"GBPUSD.MID")==0); return 0; }
