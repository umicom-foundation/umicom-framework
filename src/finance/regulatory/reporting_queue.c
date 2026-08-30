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
void umi_reg_reporting_queue_init(UmiReportingQueue *q){ if(q!=NULL){memset(q,0,sizeof *q);q->next_sequence=1U;} }
UmiStatus umi_reg_reporting_queue_push(UmiReportingQueue *q,const char *id,uint32_t p){ UmiStatus s;UmiReportingQueueItem *item;if(q==NULL||id==NULL||p==0U)return UMI_STATUS_INVALID_ARGUMENT;if(q->count>=UMI_REG_MAX_JOBS)return UMI_STATUS_CAPACITY_EXCEEDED;item=&q->items[q->count];memset(item,0,sizeof *item);s=umi_reg_copy_text(item->job_id,sizeof item->job_id,id);if(s!=UMI_STATUS_OK)return s;item->priority=p;item->sequence=q->next_sequence++;++q->count;return UMI_STATUS_OK; }
UmiStatus umi_reg_reporting_queue_pop(UmiReportingQueue *q,UmiReportingQueueItem *out){ size_t i,best;if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(q->count==0U)return UMI_STATUS_NOT_FOUND;best=0U;for(i=1U;i<q->count;++i){if(q->items[i].priority<q->items[best].priority||(q->items[i].priority==q->items[best].priority&&q->items[i].sequence<q->items[best].sequence))best=i;}*out=q->items[best];for(i=best+1U;i<q->count;++i)q->items[i-1U]=q->items[i];--q->count;return UMI_STATUS_OK; }
