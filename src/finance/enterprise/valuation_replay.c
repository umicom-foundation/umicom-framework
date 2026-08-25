/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_replay.c
 *
 * PURPOSE:
 *   Store valuation checkpoints and resolve the latest recoverable state per job.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_replay.h"

#include <string.h>
void umi_enterprise_valuation_replay_init(UmiEnterpriseValuationReplay *r){if(r!=NULL)memset(r,0,sizeof *r);}
const UmiEnterpriseValuationCheckpoint *umi_enterprise_valuation_replay_latest(const UmiEnterpriseValuationReplay *r,const char *job){size_t i;if(r==NULL||job==NULL)return NULL;for(i=r->count;i>0U;--i)if(strcmp(r->checkpoints[i-1U].job_id,job)==0)return &r->checkpoints[i-1U];return NULL;}
UmiStatus umi_enterprise_valuation_replay_append(UmiEnterpriseValuationReplay *r,const UmiEnterpriseValuationCheckpoint *c){const UmiEnterpriseValuationCheckpoint *last;if(r==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(r->count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED;last=umi_enterprise_valuation_replay_latest(r,c->job_id);if(last!=NULL&&(c->completed_tasks<last->completed_tasks||c->created_ms<=last->created_ms))return UMI_STATUS_INVALID_STATE;r->checkpoints[r->count++]=*c;return UMI_STATUS_OK;}
