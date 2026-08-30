/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_job.c
 *
 * PURPOSE:
 *   Describe one portfolio valuation request for distributed execution.
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

#include "umicom/finance/enterprise/valuation_job.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_job_init(UmiEnterpriseValuationJob *j,const char *id,const char *portfolio,int64_t asof,int priority){ UmiStatus s; if(j==NULL||asof<0||priority<0||priority>100)return UMI_STATUS_INVALID_ARGUMENT; memset(j,0,sizeof *j); s=umi_quant_copy_text(j->job_id,sizeof j->job_id,id); if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(j->portfolio_id,sizeof j->portfolio_id,portfolio); if(s!=UMI_STATUS_OK)return s; j->as_of_ms=asof;j->priority=priority;j->state=UMI_ENTERPRISE_PENDING;return UMI_STATUS_OK; }
UmiStatus umi_enterprise_valuation_job_transition(UmiEnterpriseValuationJob *j,UmiEnterpriseWorkState n){ if(j==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(j->state==UMI_ENTERPRISE_COMPLETED||j->state==UMI_ENTERPRISE_CANCELLED)return UMI_STATUS_INVALID_STATE; if(j->state==UMI_ENTERPRISE_PENDING&&(n!=UMI_ENTERPRISE_RUNNING&&n!=UMI_ENTERPRISE_CANCELLED))return UMI_STATUS_INVALID_STATE; if(j->state==UMI_ENTERPRISE_RUNNING&&(n!=UMI_ENTERPRISE_COMPLETED&&n!=UMI_ENTERPRISE_FAILED&&n!=UMI_ENTERPRISE_CANCELLED))return UMI_STATUS_INVALID_STATE; j->state=n;return UMI_STATUS_OK; }
