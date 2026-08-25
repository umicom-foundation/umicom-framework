#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/risk_report.h"

int main(void){UmiEnterpriseRiskReport r;CHECK(umi_enterprise_risk_report_init(&r,"p",1,10,12,20,-2)==UMI_STATUS_OK);CHECK(r.expected_shortfall==12.0);return 0;}
