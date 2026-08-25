#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_service.h"

int main(void){ UmiEnterpriseValuationService s; UmiEnterpriseValuationWorker w; UmiEnterpriseMarketDataUpdate u; CHECK(umi_enterprise_valuation_service_init(&s,100)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_init(&w,"w",1U)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_worker_pool_add(&s.grid.workers,&w)==UMI_STATUS_OK); CHECK(umi_enterprise_market_data_update_init(&u,"x",1.0,1U,1)==UMI_STATUS_OK); CHECK(umi_enterprise_market_data_service_publish(&s.market_data,&u)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_service_ready(&s)==1); return 0; }
