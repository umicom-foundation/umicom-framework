/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_surface_calibration_plan.c
 *
 * PURPOSE:
 *   Implement the test surface calibration plan behavior for
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

#include "umicom/finance/enterprise/surface_calibration_plan.h"

int main(void){ UmiEnterpriseSurfaceCalibrationPlan p; CHECK(umi_enterprise_surface_calibration_plan_init(&p,"SPX-VOL",40U,1e-8,200U)==UMI_STATUS_OK); CHECK(p.point_count==40U); return 0; }
