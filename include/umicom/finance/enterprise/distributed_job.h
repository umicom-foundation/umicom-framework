/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/distributed_job.h
 *
 * PURPOSE:
 *   Represent leaseable distributed work with bounded retry policy.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_DISTRIBUTED_JOB_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_DISTRIBUTED_JOB_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseDistributedJob { char job_id[UMI_ENTERPRISE_ID_CAPACITY]; char kind[UMI_ENTERPRISE_NAME_CAPACITY]; int priority; unsigned retry_count; unsigned max_retries; UmiEnterpriseWorkState state; } UmiEnterpriseDistributedJob;
/* Initialise a pending distributed job with bounded priority and retry count. */
UmiStatus umi_enterprise_distributed_job_init(UmiEnterpriseDistributedJob *job,const char *job_id,const char *kind,int priority,unsigned max_retries);
/* Record one retry while enforcing the job retry budget. */
UmiStatus umi_enterprise_distributed_job_retry(UmiEnterpriseDistributedJob *job);

#ifdef __cplusplus
}
#endif

#endif
