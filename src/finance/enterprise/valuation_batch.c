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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_batch.h"

#include <string.h>
/*
 * Initialise enterprise valuation batch from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_enterprise_valuation_batch_init(UmiEnterpriseValuationBatch *b,const char *id){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(b,0,sizeof *b); return umi_quant_copy_text(b->batch_id,sizeof b->batch_id,id); }
/*
 * Add enterprise valuation batch only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_valuation_batch_add(UmiEnterpriseValuationBatch *b,const UmiEnterpriseValuationTask *t){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL||t==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->count>0U&&strcmp(b->tasks[0].job_id,t->job_id)!=0)return UMI_STATUS_INVALID_STATE; b->tasks[b->count++]=*t; return UMI_STATUS_OK; }
/*
 * Provide the enterprise valuation batch completed operation used by this module and its
 * client applications.
 */
size_t umi_enterprise_valuation_batch_completed(const UmiEnterpriseValuationBatch *b){ size_t i,n=0U; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(b==NULL)return 0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<b->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(b->tasks[i].state==UMI_ENTERPRISE_COMPLETED)++n; return n; }
