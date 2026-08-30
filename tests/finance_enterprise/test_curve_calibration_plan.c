/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_curve_calibration_plan.c
 *
 * PURPOSE:
 *   Implement the test curve calibration plan behavior for
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

#include "umicom/finance/enterprise/curve_calibration_plan.h"

int main(void){ UmiEnterpriseCurveCalibrationPlan p; CHECK(umi_enterprise_curve_calibration_plan_init(&p,"USD-OIS",20U,1e-10,100U)==UMI_STATUS_OK); CHECK(p.max_iterations==100U); return 0; }
