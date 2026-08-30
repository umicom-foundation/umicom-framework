/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_partition.h
 *
 * PURPOSE:
 *   Represent deterministic contiguous partitions of a valuation task set.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_PARTITION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_PARTITION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseValuationPartition { size_t partition_index; size_t first_task; size_t task_count; size_t total_tasks; } UmiEnterpriseValuationPartition;
/* Initialise a partition that remains within the total task population. */
UmiStatus umi_enterprise_valuation_partition_init(UmiEnterpriseValuationPartition *partition,size_t partition_index,size_t first_task,size_t task_count,size_t total_tasks);

#ifdef __cplusplus
}
#endif

#endif
