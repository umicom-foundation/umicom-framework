/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_worker.h
 *
 * PURPOSE:
 *   Track bounded capacity and heartbeat state for a valuation worker.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_WORKER_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_WORKER_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseValuationWorker { char worker_id[UMI_ENTERPRISE_ID_CAPACITY]; size_t capacity; size_t active_tasks; int64_t heartbeat_ms; int enabled; } UmiEnterpriseValuationWorker;
/* Initialise an enabled valuation worker with fixed task capacity. */
UmiStatus umi_enterprise_valuation_worker_init(UmiEnterpriseValuationWorker *worker,const char *worker_id,size_t capacity);
/* Reserve one execution slot when capacity is available. */
UmiStatus umi_enterprise_valuation_worker_acquire(UmiEnterpriseValuationWorker *worker);
/* Release one previously acquired execution slot. */
UmiStatus umi_enterprise_valuation_worker_release(UmiEnterpriseValuationWorker *worker);

#ifdef __cplusplus
}
#endif

#endif
