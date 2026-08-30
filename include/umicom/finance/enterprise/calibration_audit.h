/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/calibration_audit.h
 *
 * PURPOSE:
 *   Record ordered calibration outcomes for operational and model-risk audit.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_AUDIT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_CALIBRATION_AUDIT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseCalibrationAuditEvent { uint64_t sequence; char job_id[UMI_ENTERPRISE_ID_CAPACITY]; double fit_error; int64_t event_time_ms; } UmiEnterpriseCalibrationAuditEvent;
typedef struct UmiEnterpriseCalibrationAudit { UmiEnterpriseCalibrationAuditEvent events[64U]; size_t count; } UmiEnterpriseCalibrationAudit;
/* Initialise an empty ordered calibration audit. */
void umi_enterprise_calibration_audit_init(UmiEnterpriseCalibrationAudit *audit);
/* Append strictly increasing audit sequence and timestamp evidence. */
UmiStatus umi_enterprise_calibration_audit_append(UmiEnterpriseCalibrationAudit *audit,const UmiEnterpriseCalibrationAuditEvent *event);

#ifdef __cplusplus
}
#endif

#endif
