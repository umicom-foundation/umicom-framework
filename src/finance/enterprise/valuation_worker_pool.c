/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_worker_pool.c
 *
 * PURPOSE:
 *   Select the least-loaded eligible worker for valuation dispatch.
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

#include "umicom/finance/enterprise/valuation_worker_pool.h"

#include <string.h>
/*
 * Initialise enterprise valuation worker pool from caller-provided values so later
 * operations receive a known state.
 */
void umi_enterprise_valuation_worker_pool_init(UmiEnterpriseValuationWorkerPool *p){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL)memset(p,0,sizeof *p); }
/*
 * Add enterprise valuation worker pool only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_enterprise_valuation_worker_pool_add(UmiEnterpriseValuationWorkerPool *p,const UmiEnterpriseValuationWorker *w){ size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||w==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(p->workers[i].worker_id,w->worker_id)==0)return UMI_STATUS_ALREADY_EXISTS; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->count>=UMI_ENTERPRISE_MAX_WORKERS)return UMI_STATUS_CAPACITY_EXCEEDED; p->workers[p->count++]=*w;return UMI_STATUS_OK; }
/*
 * Provide the enterprise valuation worker pool select operation used by this module and
 * its client applications.
 */
UmiEnterpriseValuationWorker *umi_enterprise_valuation_worker_pool_select(UmiEnterpriseValuationWorkerPool *p){ size_t i,best=0U; int found=0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL)return NULL; /* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<p->count;++i){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p->workers[i].enabled!=0&&p->workers[i].active_tasks<p->workers[i].capacity){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(!found||p->workers[i].active_tasks<p->workers[best].active_tasks){best=i;found=1;} } } return found?&p->workers[best]:NULL; }
