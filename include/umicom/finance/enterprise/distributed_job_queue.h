/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/distributed_job_queue.h
 *
 * PURPOSE:
 *   Schedule generic valuation/risk distributed jobs by stable priority.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_DISTRIBUTED_JOB_QUEUE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_DISTRIBUTED_JOB_QUEUE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/distributed_job.h"
/**
 * Represent the enterprise distributed job queue data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseDistributedJobQueue { UmiEnterpriseDistributedJob jobs[UMI_ENTERPRISE_MAX_ITEMS]; uint64_t insertion[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; uint64_t next_insertion; } UmiEnterpriseDistributedJobQueue;
/* Initialise an empty distributed-job queue. */
void umi_enterprise_distributed_job_queue_init(UmiEnterpriseDistributedJobQueue *queue);
/* Enqueue a job for stable priority dispatch. */
UmiStatus umi_enterprise_distributed_job_queue_push(UmiEnterpriseDistributedJobQueue *queue,const UmiEnterpriseDistributedJob *job);
/* Pop the highest-priority oldest job. */
UmiStatus umi_enterprise_distributed_job_queue_pop(UmiEnterpriseDistributedJobQueue *queue,UmiEnterpriseDistributedJob *out_job);

#ifdef __cplusplus
}
#endif

#endif
