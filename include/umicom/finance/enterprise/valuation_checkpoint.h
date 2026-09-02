/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_checkpoint.h
 *
 * PURPOSE:
 *   Capture recoverable progress and result fingerprints for long-running valuation jobs.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_CHECKPOINT_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_CHECKPOINT_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise valuation checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationCheckpoint { char job_id[UMI_ENTERPRISE_ID_CAPACITY]; size_t completed_tasks; size_t total_tasks; uint64_t result_fingerprint; int64_t created_ms; } UmiEnterpriseValuationCheckpoint;
/* Initialise a logically consistent recoverable valuation checkpoint. */
UmiStatus umi_enterprise_valuation_checkpoint_init(UmiEnterpriseValuationCheckpoint *checkpoint,const char *job_id,size_t completed_tasks,size_t total_tasks,uint64_t result_fingerprint,int64_t created_ms);

#ifdef __cplusplus
}
#endif

#endif
