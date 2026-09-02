/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/regulatory/reporting_queue.c
 *
 * PURPOSE:
 *   Maintain a stable bounded priority queue for regulatory reporting jobs.
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

#include "umicom/finance/regulatory/reporting_queue.h"

#include <string.h>
/*
 * Initialise reg reporting queue from caller-provided values so later operations receive a
 * known state.
 */
void umi_reg_reporting_queue_init(UmiReportingQueue *q){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(q!=NULL){memset(q,0,sizeof *q);q->next_sequence=1U;} }
/*
 * Provide the reg reporting queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_reporting_queue_push(UmiReportingQueue *q,const char *id,uint32_t p){ UmiStatus s;UmiReportingQueueItem *item;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||id==NULL||p==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count>=UMI_REG_MAX_JOBS)return UMI_STATUS_CAPACITY_EXCEEDED;item=&q->items[q->count];memset(item,0,sizeof *item);s=umi_reg_copy_text(item->job_id,sizeof item->job_id,id);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s;item->priority=p;item->sequence=q->next_sequence++;++q->count;return UMI_STATUS_OK; }
/*
 * Provide the reg reporting queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_reg_reporting_queue_pop(UmiReportingQueue *q,UmiReportingQueueItem *out){ size_t i,best;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count==0U)return UMI_STATUS_NOT_FOUND;best=0U;/* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<q->count;++i){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->items[i].priority<q->items[best].priority||(q->items[i].priority==q->items[best].priority&&q->items[i].sequence<q->items[best].sequence))best=i;}*out=q->items[best];/* Visit each bounded item once so every record receives the same rule. */ for(i=best+1U;i<q->count;++i)q->items[i-1U]=q->items[i];--q->count;return UMI_STATUS_OK; }
