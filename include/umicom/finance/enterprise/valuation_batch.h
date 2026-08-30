/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_batch.h
 *
 * PURPOSE:
 *   Group bounded instrument valuation tasks for coherent dispatch.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_BATCH_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_BATCH_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_task.h"
typedef struct UmiEnterpriseValuationBatch { char batch_id[UMI_ENTERPRISE_ID_CAPACITY]; UmiEnterpriseValuationTask tasks[UMI_ENTERPRISE_MAX_ITEMS]; size_t count; } UmiEnterpriseValuationBatch;
/* Initialise an empty valuation batch. */
UmiStatus umi_enterprise_valuation_batch_init(UmiEnterpriseValuationBatch *batch,const char *batch_id);
/* Append a task while enforcing a common job identity across the batch. */
UmiStatus umi_enterprise_valuation_batch_add(UmiEnterpriseValuationBatch *batch,const UmiEnterpriseValuationTask *task);
/* Count terminally completed tasks for progress reporting. */
size_t umi_enterprise_valuation_batch_completed(const UmiEnterpriseValuationBatch *batch);

#ifdef __cplusplus
}
#endif

#endif
