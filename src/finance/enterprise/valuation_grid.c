/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_grid.c
 *
 * PURPOSE:
 *   Coordinate valuation queueing, worker selection and dispatch accounting.
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

#include "umicom/finance/enterprise/valuation_grid.h"

#include <string.h>
/*
 * Initialise enterprise valuation grid from caller-provided values so later operations
 * receive a known state.
 */
void umi_enterprise_valuation_grid_init(UmiEnterpriseValuationGrid *g){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g!=NULL){memset(g,0,sizeof *g);umi_enterprise_valuation_queue_init(&g->queue);umi_enterprise_valuation_worker_pool_init(&g->workers);} }
/*
 * Perform enterprise valuation grid through the module contract so client applications do
 * not duplicate its policy.
 */
UmiStatus umi_enterprise_valuation_grid_dispatch(UmiEnterpriseValuationGrid *g,UmiEnterpriseValuationJob *job,UmiEnterpriseValuationWorker **worker){ UmiStatus s; UmiEnterpriseValuationWorker *w; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(g==NULL||job==NULL||worker==NULL)return UMI_STATUS_INVALID_ARGUMENT; w=umi_enterprise_valuation_worker_pool_select(&g->workers); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(w==NULL){g->blocked_dispatches++;return UMI_STATUS_BUSY;} s=umi_enterprise_valuation_queue_pop(&g->queue,job); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; s=umi_enterprise_valuation_worker_acquire(w); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=UMI_STATUS_OK)return s; *worker=w; g->dispatched_jobs++; return UMI_STATUS_OK; }
