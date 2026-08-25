/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_queue.c
 *
 * PURPOSE:
 *   Schedule valuation jobs by priority with stable FIFO tie-breaking.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_queue.h"

#include <string.h>
void umi_enterprise_valuation_queue_init(UmiEnterpriseValuationQueue *q){ if(q!=NULL){memset(q,0,sizeof *q);q->next_insertion=1U;} }
UmiStatus umi_enterprise_valuation_queue_push(UmiEnterpriseValuationQueue *q,const UmiEnterpriseValuationJob *j){ if(q==NULL||j==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(q->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; q->jobs[q->count]=*j; q->insertion[q->count]=q->next_insertion++; q->count++; return UMI_STATUS_OK; }
UmiStatus umi_enterprise_valuation_queue_pop(UmiEnterpriseValuationQueue *q,UmiEnterpriseValuationJob *out){ size_t i,best; if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT; if(q->count==0U)return UMI_STATUS_NOT_FOUND; best=0U; for(i=1U;i<q->count;++i)if(q->jobs[i].priority>q->jobs[best].priority||(q->jobs[i].priority==q->jobs[best].priority&&q->insertion[i]<q->insertion[best]))best=i; *out=q->jobs[best]; for(i=best+1U;i<q->count;++i){q->jobs[i-1U]=q->jobs[i];q->insertion[i-1U]=q->insertion[i];} q->count--; return UMI_STATUS_OK; }
