/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_grid.h
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_GRID_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_GRID_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_queue.h"
#include "umicom/finance/enterprise/valuation_worker_pool.h"
typedef struct UmiEnterpriseValuationGrid { UmiEnterpriseValuationQueue queue; UmiEnterpriseValuationWorkerPool workers; uint64_t dispatched_jobs; uint64_t blocked_dispatches; } UmiEnterpriseValuationGrid;
/* Initialise an empty valuation grid. */
void umi_enterprise_valuation_grid_init(UmiEnterpriseValuationGrid *grid);
/* Dispatch the next valuation job to an available worker, reserving its capacity. */
UmiStatus umi_enterprise_valuation_grid_dispatch(UmiEnterpriseValuationGrid *grid,UmiEnterpriseValuationJob *out_job,UmiEnterpriseValuationWorker **out_worker);

#ifdef __cplusplus
}
#endif

#endif
