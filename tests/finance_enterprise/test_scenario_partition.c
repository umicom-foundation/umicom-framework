#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/scenario_partition.h"

int main(void){UmiEnterpriseScenarioPartition p;CHECK(umi_enterprise_scenario_partition_init(&p,1U,10U,5U,20U)==UMI_STATUS_OK);CHECK(p.scenario_count==5U);return 0;}
