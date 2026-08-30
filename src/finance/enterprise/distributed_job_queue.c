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
void umi_enterprise_distributed_job_queue_init(UmiEnterpriseDistributedJobQueue *q){if(q!=NULL){memset(q,0,sizeof *q);q->next_insertion=1U;}}
UmiStatus umi_enterprise_distributed_job_queue_push(UmiEnterpriseDistributedJobQueue *q,const UmiEnterpriseDistributedJob *j){if(q==NULL||j==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(q->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;q->jobs[q->count]=*j;q->insertion[q->count]=q->next_insertion++;q->count++;return UMI_STATUS_OK;}
UmiStatus umi_enterprise_distributed_job_queue_pop(UmiEnterpriseDistributedJobQueue *q,UmiEnterpriseDistributedJob *out){size_t i,best;if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(q->count==0U)return UMI_STATUS_NOT_FOUND;best=0U;for(i=1U;i<q->count;++i)if(q->jobs[i].priority>q->jobs[best].priority||(q->jobs[i].priority==q->jobs[best].priority&&q->insertion[i]<q->insertion[best]))best=i;*out=q->jobs[best];for(i=best+1U;i<q->count;++i){q->jobs[i-1U]=q->jobs[i];q->insertion[i-1U]=q->insertion[i];}q->count--;return UMI_STATUS_OK;}
