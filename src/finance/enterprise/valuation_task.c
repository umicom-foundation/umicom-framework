/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_task.c
 *
 * PURPOSE:
 *   Represent one instrument-level valuation task within a job.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_task.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_task_init(UmiEnterpriseValuationTask *t,const char *tid,const char *jid,const char *pid,double q){ UmiStatus s; if(t==NULL||!umi_quant_number_valid(q)||q==0.0)return UMI_STATUS_INVALID_ARGUMENT; memset(t,0,sizeof *t); s=umi_quant_copy_text(t->task_id,sizeof t->task_id,tid); if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(t->job_id,sizeof t->job_id,jid); if(s!=UMI_STATUS_OK)return s; s=umi_quant_copy_text(t->product_id.value,sizeof t->product_id.value,pid); if(s!=UMI_STATUS_OK)return s; t->quantity=q;t->state=UMI_ENTERPRISE_PENDING;return UMI_STATUS_OK; }
