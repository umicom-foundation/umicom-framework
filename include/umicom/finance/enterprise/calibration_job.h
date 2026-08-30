/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/calibration_job.h
 *
 * PURPOSE:
 *   Track one curve or surface calibration work item.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_JOB_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_JOB_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseCalibrationJob { char job_id[UMI_ENTERPRISE_ID_CAPACITY]; char object_id[UMI_ENTERPRISE_ID_CAPACITY]; UmiEnterpriseWorkState state; size_t target_count; size_t completed_targets; } UmiEnterpriseCalibrationJob;
/* Initialise one pending calibration job. */
UmiStatus umi_enterprise_calibration_job_init(UmiEnterpriseCalibrationJob *job,const char *job_id,const char *object_id,size_t target_count);
/* Advance completed target evidence without exceeding the declared target count. */
UmiStatus umi_enterprise_calibration_job_progress(UmiEnterpriseCalibrationJob *job,size_t completed_targets);

#ifdef __cplusplus
}
#endif

#endif
