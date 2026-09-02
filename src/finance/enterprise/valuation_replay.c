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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_replay.h"

#include <string.h>
/*
 * Initialise enterprise valuation replay from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_valuation_replay_init(UmiEnterpriseValuationReplay *r){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r!=NULL)memset(r,0,sizeof *r);}
/*
 * Provide the enterprise valuation replay latest operation used by this module and its
 * client applications.
 */
const UmiEnterpriseValuationCheckpoint *umi_enterprise_valuation_replay_latest(const UmiEnterpriseValuationReplay *r,const char *job){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||job==NULL)return NULL;/* Visit each bounded item once so every record receives the same rule. */ for(i=r->count;i>0U;--i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(r->checkpoints[i-1U].job_id,job)==0)return &r->checkpoints[i-1U];return NULL;}
/*
 * Add enterprise valuation replay only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_enterprise_valuation_replay_append(UmiEnterpriseValuationReplay *r,const UmiEnterpriseValuationCheckpoint *c){const UmiEnterpriseValuationCheckpoint *last;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||c==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r->count>=64U)return UMI_STATUS_CAPACITY_EXCEEDED;last=umi_enterprise_valuation_replay_latest(r,c->job_id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(last!=NULL&&(c->completed_tasks<last->completed_tasks||c->created_ms<=last->created_ms))return UMI_STATUS_INVALID_STATE;r->checkpoints[r->count++]=*c;return UMI_STATUS_OK;}
