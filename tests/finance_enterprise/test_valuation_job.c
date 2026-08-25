#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/valuation_job.h"

int main(void){ UmiEnterpriseValuationJob j; CHECK(umi_enterprise_valuation_job_init(&j,"j","p",10,50)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_job_transition(&j,UMI_ENTERPRISE_RUNNING)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_job_transition(&j,UMI_ENTERPRISE_COMPLETED)==UMI_STATUS_OK); return 0; }
