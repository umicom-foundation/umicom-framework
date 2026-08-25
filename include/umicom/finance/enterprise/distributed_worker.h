/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/distributed_worker.h
 *
 * PURPOSE:
 *   Track heartbeat, capacity and lease evidence for distributed valuation workers.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_DISTRIBUTED_WORKER_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_DISTRIBUTED_WORKER_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseDistributedWorker { char worker_id[UMI_ENTERPRISE_ID_CAPACITY]; size_t capacity; size_t active_jobs; int64_t heartbeat_ms; int64_t lease_until_ms; } UmiEnterpriseDistributedWorker;
/* Initialise one distributed worker with fixed capacity. */
UmiStatus umi_enterprise_distributed_worker_init(UmiEnterpriseDistributedWorker *worker,const char *worker_id,size_t capacity);
/* Refresh worker heartbeat and lease expiry while enforcing forward time. */
UmiStatus umi_enterprise_distributed_worker_heartbeat(UmiEnterpriseDistributedWorker *worker,int64_t heartbeat_ms,int64_t lease_until_ms);
/* Return one when the worker lease is valid and spare capacity exists. */
int umi_enterprise_distributed_worker_available(const UmiEnterpriseDistributedWorker *worker,int64_t now_ms);

#ifdef __cplusplus
}
#endif

#endif
