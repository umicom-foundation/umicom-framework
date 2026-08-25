/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_batch.c
 *
 * PURPOSE:
 *   Group bounded instrument valuation tasks for coherent dispatch.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_batch.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_batch_init(UmiEnterpriseValuationBatch *b,const char *id){ if(b==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(b,0,sizeof *b); return umi_quant_copy_text(b->batch_id,sizeof b->batch_id,id); }
UmiStatus umi_enterprise_valuation_batch_add(UmiEnterpriseValuationBatch *b,const UmiEnterpriseValuationTask *t){ if(b==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(b->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; if(b->count>0U&&strcmp(b->tasks[0].job_id,t->job_id)!=0)return UMI_STATUS_INVALID_STATE; b->tasks[b->count++]=*t; return UMI_STATUS_OK; }
size_t umi_enterprise_valuation_batch_completed(const UmiEnterpriseValuationBatch *b){ size_t i,n=0U; if(b==NULL)return 0U; for(i=0U;i<b->count;++i)if(b->tasks[i].state==UMI_ENTERPRISE_COMPLETED)++n; return n; }
