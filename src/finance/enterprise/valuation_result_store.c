/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_result_store.c
 *
 * PURPOSE:
 *   Persist bounded instrument-level valuation results for aggregation and replay.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_result_store.h"

#include <string.h>
void umi_enterprise_valuation_result_store_init(UmiEnterpriseValuationResultStore *s){ if(s!=NULL)memset(s,0,sizeof *s); }
UmiStatus umi_enterprise_valuation_result_store_append(UmiEnterpriseValuationResultStore *s,const UmiEnterpriseValuationResultRecord *r){ size_t i; if(s==NULL||r==NULL||!umi_quant_number_valid(r->present_value))return UMI_STATUS_INVALID_ARGUMENT; for(i=0U;i<s->count;++i)if(strcmp(s->records[i].task_id,r->task_id)==0)return UMI_STATUS_ALREADY_EXISTS; if(s->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; s->records[s->count++]=*r;return UMI_STATUS_OK; }
double umi_enterprise_valuation_result_store_job_pv(const UmiEnterpriseValuationResultStore *s,const char *job){ size_t i; double total=0.0; if(s==NULL||job==NULL)return 0.0; for(i=0U;i<s->count;++i)if(s->records[i].status==UMI_STATUS_OK&&strcmp(s->records[i].job_id,job)==0)total+=s->records[i].present_value; return total; }
