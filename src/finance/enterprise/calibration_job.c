/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/calibration_job.c
 *
 * PURPOSE:
 *   Track one curve or surface calibration work item.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/calibration_job.h"

#include <string.h>
UmiStatus umi_enterprise_calibration_job_init(UmiEnterpriseCalibrationJob *j,const char *id,const char *obj,size_t targets){ UmiStatus s; if(j==NULL||targets==0U)return UMI_STATUS_INVALID_ARGUMENT; memset(j,0,sizeof *j); s=umi_quant_copy_text(j->job_id,sizeof j->job_id,id); if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(j->object_id,sizeof j->object_id,obj); if(s!=UMI_STATUS_OK)return s; j->target_count=targets; j->state=UMI_ENTERPRISE_PENDING; return UMI_STATUS_OK; }
UmiStatus umi_enterprise_calibration_job_progress(UmiEnterpriseCalibrationJob *j,size_t completed){ if(j==NULL||completed>j->target_count)return UMI_STATUS_INVALID_ARGUMENT; j->completed_targets=completed; j->state=(completed==j->target_count)?UMI_ENTERPRISE_COMPLETED:UMI_ENTERPRISE_RUNNING; return UMI_STATUS_OK; }
