/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/finance/enterprise/scenario_partition.c
 *
 * PURPOSE:
 *   Represent deterministic contiguous scenario partitions for distributed execution.
 *
 * ARCHITECTURE:
 *   This reusable capability is Framework-owned; financial applications remain thin compositions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/finance/enterprise/scenario_partition.h"

UmiStatus umi_enterprise_scenario_partition_init(UmiEnterpriseScenarioPartition *p,size_t index,size_t first,size_t count,size_t total){if(p==NULL||count==0U||total==0U||first>=total||count>total-first)return UMI_STATUS_INVALID_ARGUMENT;p->partition_index=index;p->first_scenario=first;p->scenario_count=count;p->total_scenarios=total;return UMI_STATUS_OK;}
