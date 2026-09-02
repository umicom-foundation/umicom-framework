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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/valuation_queue.h"

#include <string.h>
/*
 * Initialise enterprise valuation queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_valuation_queue_init(UmiEnterpriseValuationQueue *q){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(q!=NULL){memset(q,0,sizeof *q);q->next_insertion=1U;} }
/*
 * Provide the enterprise valuation queue push operation used by this module and its client
 * applications.
 */
UmiStatus umi_enterprise_valuation_queue_push(UmiEnterpriseValuationQueue *q,const UmiEnterpriseValuationJob *j){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||j==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count>=UMI_ENTERPRISE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED; q->jobs[q->count]=*j; q->insertion[q->count]=q->next_insertion++; q->count++; return UMI_STATUS_OK; }
/*
 * Provide the enterprise valuation queue pop operation used by this module and its client
 * applications.
 */
UmiStatus umi_enterprise_valuation_queue_pop(UmiEnterpriseValuationQueue *q,UmiEnterpriseValuationJob *out){ size_t i,best; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(q==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->count==0U)return UMI_STATUS_NOT_FOUND; best=0U; /* Visit each bounded item once so every record receives the same rule. */ for(i=1U;i<q->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(q->jobs[i].priority>q->jobs[best].priority||(q->jobs[i].priority==q->jobs[best].priority&&q->insertion[i]<q->insertion[best]))best=i; *out=q->jobs[best]; /* Visit each bounded item once so every record receives the same rule. */ for(i=best+1U;i<q->count;++i){q->jobs[i-1U]=q->jobs[i];q->insertion[i-1U]=q->insertion[i];} q->count--; return UMI_STATUS_OK; }
