#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/historical_scenario.h"

int main(void){ UmiEnterpriseHistoricalScenario s;CHECK(umi_enterprise_historical_scenario_init(&s,"2008")==UMI_STATUS_OK);CHECK(umi_enterprise_historical_scenario_add_shock(&s,"SPX",-0.1)==UMI_STATUS_OK);CHECK(s.count==1U);return 0; }
