/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/scenario_partition.h
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_SCENARIO_PARTITION_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_SCENARIO_PARTITION_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEnterpriseScenarioPartition { size_t partition_index; size_t first_scenario; size_t scenario_count; size_t total_scenarios; } UmiEnterpriseScenarioPartition;
/* Initialise a scenario partition that remains inside the scenario grid. */
UmiStatus umi_enterprise_scenario_partition_init(UmiEnterpriseScenarioPartition *partition,size_t partition_index,size_t first_scenario,size_t scenario_count,size_t total_scenarios);

#ifdef __cplusplus
}
#endif

#endif
