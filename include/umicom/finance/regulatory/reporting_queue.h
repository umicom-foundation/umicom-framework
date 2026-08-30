/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/regulatory/reporting_queue.h
 *
 * PURPOSE:
 *   Maintain a stable bounded priority queue for regulatory reporting jobs.
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

#ifndef INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_QUEUE_H
#define INCLUDE_UMICOM_FINANCE_REGULATORY_REPORTING_QUEUE_H

#include "umicom/finance/regulatory/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiReportingQueueItem { char job_id[UMI_REG_ID_CAPACITY]; uint32_t priority; uint64_t sequence; } UmiReportingQueueItem;
typedef struct UmiReportingQueue { UmiReportingQueueItem items[UMI_REG_MAX_JOBS]; size_t count; uint64_t next_sequence; } UmiReportingQueue;
/* Initialise an empty stable priority queue. */
void umi_reg_reporting_queue_init(UmiReportingQueue *queue);
/* Enqueue one reporting job using lower numeric priority as more urgent. */
UmiStatus umi_reg_reporting_queue_push(UmiReportingQueue *queue,const char *job_id,uint32_t priority);
/* Pop the highest-priority oldest reporting job. */
UmiStatus umi_reg_reporting_queue_pop(UmiReportingQueue *queue,UmiReportingQueueItem *out_item);

#ifdef __cplusplus
}
#endif

#endif
