#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/risk_cube.h"

int main(void){ UmiEnterpriseRiskCube c; CHECK(umi_enterprise_risk_cube_init(&c,2U,2U)==UMI_STATUS_OK); CHECK(umi_enterprise_risk_cube_set(&c,0U,0U,1.0)==UMI_STATUS_OK); CHECK(umi_enterprise_risk_cube_set(&c,0U,1U,2.0)==UMI_STATUS_OK); CHECK(umi_enterprise_risk_cube_scenario_total(&c,0U)==3.0); return 0; }
