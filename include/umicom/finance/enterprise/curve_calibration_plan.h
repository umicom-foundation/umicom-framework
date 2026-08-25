/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/curve_calibration_plan.h
 *
 * PURPOSE:
 *   Govern calibration inputs, tolerance and iteration limits for one curve.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_CURVE_CALIBRATION_PLAN_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_CURVE_CALIBRATION_PLAN_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseCurveCalibrationPlan { char curve_id[UMI_ENTERPRISE_ID_CAPACITY]; size_t instrument_count; double tolerance; size_t max_iterations; } UmiEnterpriseCurveCalibrationPlan;
/* Initialise a numerically valid bounded curve-calibration plan. */
UmiStatus umi_enterprise_curve_calibration_plan_init(UmiEnterpriseCurveCalibrationPlan *plan,const char *curve_id,size_t instrument_count,double tolerance,size_t max_iterations);

#ifdef __cplusplus
}
#endif

#endif
