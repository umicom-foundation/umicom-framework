/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_worker_pool.h
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_WORKER_POOL_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_WORKER_POOL_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_worker.h"
typedef struct UmiEnterpriseValuationWorkerPool { UmiEnterpriseValuationWorker workers[UMI_ENTERPRISE_MAX_WORKERS]; size_t count; } UmiEnterpriseValuationWorkerPool;
/* Initialise an empty worker pool. */
void umi_enterprise_valuation_worker_pool_init(UmiEnterpriseValuationWorkerPool *pool);
/* Add a uniquely identified worker. */
UmiStatus umi_enterprise_valuation_worker_pool_add(UmiEnterpriseValuationWorkerPool *pool,const UmiEnterpriseValuationWorker *worker);
/* Return the least-loaded enabled worker with spare capacity. */
UmiEnterpriseValuationWorker *umi_enterprise_valuation_worker_pool_select(UmiEnterpriseValuationWorkerPool *pool);

#ifdef __cplusplus
}
#endif

#endif
