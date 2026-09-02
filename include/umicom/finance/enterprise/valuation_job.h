/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_job.h
 *
 * PURPOSE:
 *   Describe one portfolio valuation request for distributed execution.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_JOB_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_JOB_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the enterprise valuation job data shared with callers of this public contract.
 */
typedef struct UmiEnterpriseValuationJob { char job_id[UMI_ENTERPRISE_ID_CAPACITY]; char portfolio_id[UMI_ENTERPRISE_ID_CAPACITY]; int64_t as_of_ms; int priority; UmiEnterpriseWorkState state; } UmiEnterpriseValuationJob;
/* Initialise a pending valuation job with bounded priority. */
UmiStatus umi_enterprise_valuation_job_init(UmiEnterpriseValuationJob *job,const char *job_id,const char *portfolio_id,int64_t as_of_ms,int priority);
/* Apply a legal work-state transition to the valuation job. */
UmiStatus umi_enterprise_valuation_job_transition(UmiEnterpriseValuationJob *job,UmiEnterpriseWorkState next_state);

#ifdef __cplusplus
}
#endif

#endif
