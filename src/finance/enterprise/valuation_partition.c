/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/valuation_partition.c
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

#include "umicom/finance/enterprise/valuation_partition.h"

/*
 * Initialise enterprise valuation partition from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_enterprise_valuation_partition_init(UmiEnterpriseValuationPartition *p,size_t index,size_t first,size_t count,size_t total){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(p==NULL||count==0U||total==0U||first>=total||count>total-first)return UMI_STATUS_INVALID_ARGUMENT; p->partition_index=index;p->first_task=first;p->task_count=count;p->total_tasks=total;return UMI_STATUS_OK; }
