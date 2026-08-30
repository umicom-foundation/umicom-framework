/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_checkpoint.c
 *
 * PURPOSE:
 *   Capture recoverable progress and result fingerprints for long-running valuation jobs.
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

#include "umicom/finance/enterprise/valuation_checkpoint.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_checkpoint_init(UmiEnterpriseValuationCheckpoint *c,const char *job,size_t completed,size_t total,uint64_t fp,int64_t created){UmiStatus s;if(c==NULL||total==0U||completed>total||created<0)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof *c);s=umi_quant_copy_text(c->job_id,sizeof c->job_id,job);if(s!=UMI_STATUS_OK)return s;c->completed_tasks=completed;c->total_tasks=total;c->result_fingerprint=fp;c->created_ms=created;return UMI_STATUS_OK;}
