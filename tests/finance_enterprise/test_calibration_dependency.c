/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_calibration_dependency.c
 *
 * PURPOSE:
 *   Implement the test calibration dependency behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/enterprise/calibration_dependency.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseCalibrationDependency d; CHECK(umi_enterprise_calibration_dependency_init(&d,"discount","forward")==UMI_STATUS_OK); CHECK(umi_enterprise_calibration_dependency_init(&d,"x","x")==UMI_STATUS_INVALID_ARGUMENT); return 0; }
