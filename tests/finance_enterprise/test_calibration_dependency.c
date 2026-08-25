#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/calibration_dependency.h"

int main(void){ UmiEnterpriseCalibrationDependency d; CHECK(umi_enterprise_calibration_dependency_init(&d,"discount","forward")==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_dependency_init(&d,"x","x")==UMI_STATUS_INVALID_ARGUMENT); return 0; }
