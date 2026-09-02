/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_queue.h
 *
 * PURPOSE:
 *   Schedule valuation jobs by priority with stable FIFO tie-breaking.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_QUEUE_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_QUEUE_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_job.h"
/**
 * Represent the enterprise valuation queue data shared with callers of this public
 * contract.
 */
typedef struct UmiEnterpriseValuationQueue { UmiEnterpriseValuationJob jobs[UMI_ENTERPRISE_MAX_ITEMS]; uint64_t insertion[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; uint64_t next_insertion; } UmiEnterpriseValuationQueue;
/* Initialise an empty stable valuation queue. */
void umi_enterprise_valuation_queue_init(UmiEnterpriseValuationQueue *queue);
/* Enqueue one valuation job. */
UmiStatus umi_enterprise_valuation_queue_push(UmiEnterpriseValuationQueue *queue,const UmiEnterpriseValuationJob *job);
/* Remove the highest-priority oldest job. */
UmiStatus umi_enterprise_valuation_queue_pop(UmiEnterpriseValuationQueue *queue,UmiEnterpriseValuationJob *out_job);

#ifdef __cplusplus
}
#endif

#endif
