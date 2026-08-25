#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/stress_scenario.h"

int main(void){UmiEnterpriseStressScenario s;CHECK(umi_enterprise_stress_scenario_init(&s,"rates+200","USD",0.02,4)==UMI_STATUS_OK);CHECK(s.severity==4);return 0;}
