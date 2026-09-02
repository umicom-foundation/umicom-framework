/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/distributed_job_queue.c
 *
 * PURPOSE:
 *   Schedule generic valuation/risk distributed jobs by stable priority.
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

#include "umicom/finance/enterprise/distributed_job_queue.h"

#include <string.h>
/*
 * Initialise enterprise distributed job queue from caller-provided values so later
 * operations receive a known state.
 */
void umi_enterprise_distributed_job_queue_init(UmiEnterpriseDistributedJobQueue *q){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q!=NULL){memset(q,0,sizeof *q);q->next_insertion=1U;}}
/*
 * Provide the enterprise distributed job queue push operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_distributed_job_queue_push(UmiEnterpriseDistributedJobQueue *q,const UmiEnterpriseDistributedJob *j){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||j==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;q->jobs[q->count]=*j;q->insertion[q->count]=q->next_insertion++;q->count++;return UMI_STATUS_OK;}
/*
 * Provide the enterprise distributed job queue pop operation used by this module and its
 * client applications.
 */
UmiStatus umi_enterprise_distributed_job_queue_pop(UmiEnterpriseDistributedJobQueue *q,UmiEnterpriseDistributedJob *out){size_t i,best;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count==0U)return UMI_STATUS_NOT_FOUND;best=0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<q->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->jobs[i].priority>q->jobs[best].priority||(q->jobs[i].priority==q->jobs[best].priority&&q->insertion[i]<q->insertion[best]))best=i;*out=q->jobs[best];/* Visit each bounded item once so every record receives the same rule. */ for(i=best+1U;i<q->count;++i){q->jobs[i-1U]=q->jobs[i];q->insertion[i-1U]=q->insertion[i];}q->count--;return UMI_STATUS_OK;}
