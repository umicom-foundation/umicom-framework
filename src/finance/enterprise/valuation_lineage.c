/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_lineage.c
 *
 * PURPOSE:
 *   Capture reproducible market/model/source lineage for a valuation job.
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

#include "umicom/finance/enterprise/valuation_lineage.h"

#include <string.h>
UmiStatus umi_enterprise_valuation_lineage_init(UmiEnterpriseValuationLineage *l,const char *job,uint64_t market,const char *model,const char *rev){UmiStatus s;uint64_t h;if(l==NULL||market==0U)return UMI_STATUS_INVALID_ARGUMENT;memset(l,0,sizeof *l);s=umi_quant_copy_text(l->job_id,sizeof l->job_id,job);if(s!=UMI_STATUS_OK)return s;s=umi_quant_copy_text(l->model_id,sizeof l->model_id,model);if(s!=UMI_STATUS_OK)return s;s=umi_quant_copy_text(l->source_revision,sizeof l->source_revision,rev);if(s!=UMI_STATUS_OK)return s;l->market_fingerprint=market;h=umi_enterprise_hash_text(l->job_id)^market^umi_enterprise_hash_text(l->model_id)^umi_enterprise_hash_text(l->source_revision);l->lineage_fingerprint=h;return UMI_STATUS_OK;}
