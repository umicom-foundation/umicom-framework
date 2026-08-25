/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/calibration_audit.c
 *
 * PURPOSE:
 *   Record ordered calibration outcomes for operational and model-risk audit.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/calibration_audit.h"

#include <string.h>
void umi_enterprise_calibration_audit_init(UmiEnterpriseCalibrationAudit *a){ if(a!=NULL)memset(a,0,sizeof *a); }
UmiStatus umi_enterprise_calibration_audit_append(UmiEnterpriseCalibrationAudit *a,const UmiEnterpriseCalibrationAuditEvent *e){ if(a==NULL||e==NULL||e->sequence==0U||!umi_quant_number_valid(e->fit_error)||e->fit_error<0.0)return UMI_STATUS_INVALID_ARGUMENT; if(a->count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED; if(a->count>0U&&(e->sequence<=a->events[a->count-1U].sequence||e->event_time_ms<a->events[a->count-1U].event_time_ms))return UMI_STATUS_INVALID_STATE; a->events[a->count++]=*e; return UMI_STATUS_OK; }
