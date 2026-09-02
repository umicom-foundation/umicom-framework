/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/surface_calibration_plan.h
 *
 * PURPOSE:
 *   Govern calibration inputs, tolerance and iteration limits for one volatility surface.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_SURFACE_CALIBRATION_PLAN_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_SURFACE_CALIBRATION_PLAN_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise surface calibration plan data shared with callers of this
 * public contract.
 */
typedef struct UmiEnterpriseSurfaceCalibrationPlan { char surface_id[UMI_ENTERPRISE_ID_CAPACITY]; size_t point_count; double tolerance; size_t max_iterations; } UmiEnterpriseSurfaceCalibrationPlan;
/* Initialise a numerically valid volatility-surface calibration plan. */
UmiStatus umi_enterprise_surface_calibration_plan_init(UmiEnterpriseSurfaceCalibrationPlan *plan,const char *surface_id,size_t point_count,double tolerance,size_t max_iterations);

#ifdef __cplusplus
}
#endif

#endif
