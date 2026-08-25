#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/risk_snapshot.h"

int main(void){UmiEnterpriseRiskSnapshot s;UmiEnterpriseRiskReport r;CHECK(umi_enterprise_risk_snapshot_init(&s,1)==UMI_STATUS_OK);CHECK(umi_enterprise_risk_report_init(&r,"p",1,10,12,20,0)==UMI_STATUS_OK);CHECK(umi_enterprise_risk_snapshot_add(&s,&r)==UMI_STATUS_OK);CHECK(umi_enterprise_risk_snapshot_sum_var(&s)==10.0);return 0;}
