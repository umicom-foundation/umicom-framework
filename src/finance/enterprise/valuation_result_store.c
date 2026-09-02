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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_result_store.h"

#include <string.h>
/*
 * Initialise enterprise valuation result store from caller-provided values so later
 * operations receive a known state.
 */
void umi_enterprise_valuation_result_store_init(UmiEnterpriseValuationResultStore *s){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s); }
/*
 * Add enterprise valuation result store only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_enterprise_valuation_result_store_append(UmiEnterpriseValuationResultStore *s,const UmiEnterpriseValuationResultRecord *r){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||r==NULL||!umi_quant_number_valid(r->present_value))return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(s->records[i].task_id,r->task_id)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; s->records[s->count++]=*r;return UMI_STATUS_OK; }
/*
 * Provide the enterprise valuation result store job pv operation used by this module and
 * its client applications.
 */
double umi_enterprise_valuation_result_store_job_pv(const UmiEnterpriseValuationResultStore *s,const char *job){ size_t i; double total=0.0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||job==NULL)return 0.0; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<s->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->records[i].status==UMI_STATUS_OK&&strcmp(s->records[i].job_id,job)==0)total+=s->records[i].present_value; return total; }
