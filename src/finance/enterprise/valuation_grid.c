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
void umi_enterprise_valuation_grid_init(UmiEnterpriseValuationGrid *g){ if(g!=NULL){memset(g,0,sizeof *g);umi_enterprise_valuation_queue_init(&g->queue);umi_enterprise_valuation_worker_pool_init(&g->workers);} }
UmiStatus umi_enterprise_valuation_grid_dispatch(UmiEnterpriseValuationGrid *g,UmiEnterpriseValuationJob *job,UmiEnterpriseValuationWorker **worker){ UmiStatus s; UmiEnterpriseValuationWorker *w; if(g==NULL||job==NULL||worker==NULL)return UMI_STATUS_INVALID_ARGUMENT; w=umi_enterprise_valuation_worker_pool_select(&g->workers); if(w==NULL){g->blocked_dispatches++;return UMI_STATUS_BUSY;} s=umi_enterprise_valuation_queue_pop(&g->queue,job); if(s!=UMI_STATUS_OK)return s; s=umi_enterprise_valuation_worker_acquire(w); if(s!=UMI_STATUS_OK)return s; *worker=w; g->dispatched_jobs++; return UMI_STATUS_OK; }
