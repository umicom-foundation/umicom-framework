#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/calibration_job.h"

int main(void){ UmiEnterpriseCalibrationJob j; CHECK(umi_enterprise_calibration_job_init(&j,"j","curve",2U)==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_job_progress(&j,2U)==UMI_STATUS_OK); CHECK(j.state==UMI_ENTERPRISE_COMPLETED); return 0; }
