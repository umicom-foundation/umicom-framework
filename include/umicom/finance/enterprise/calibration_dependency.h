/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/calibration_dependency.h
 *
 * PURPOSE:
 *   Describe ordering dependencies between calibration jobs.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_DEPENDENCY_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_DEPENDENCY_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseCalibrationDependency { char prerequisite_id[UMI_ENTERPRISE_ID_CAPACITY]; char dependent_id[UMI_ENTERPRISE_ID_CAPACITY]; } UmiEnterpriseCalibrationDependency;
/* Initialise a directed calibration dependency while rejecting self-dependency. */
UmiStatus umi_enterprise_calibration_dependency_init(UmiEnterpriseCalibrationDependency *dependency,const char *prerequisite_id,const char *dependent_id);

#ifdef __cplusplus
}
#endif

#endif
