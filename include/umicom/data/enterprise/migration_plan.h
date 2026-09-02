/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_plan.h
 *
 * PURPOSE:
 *   Produce a deterministic topological migration order from a validated dependency graph.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_PLAN_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_PLAN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/migration_graph.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data migration plan data shared with callers of this public contract.
 */
typedef struct UmiDataMigrationPlan { char step_ids[UMI_DATA_ENTERPRISE_MAX_ITEMS][UMI_DATA_ENTERPRISE_ID_CAPACITY]; size_t count; bool reversible; bool destructive; } UmiDataMigrationPlan;
/* Build a topological migration plan or reject cyclic graphs. */ UmiStatus umi_data_migration_plan_build(const UmiDataMigrationGraph *graph,UmiDataMigrationPlan *out_plan);

#ifdef __cplusplus
}
#endif
#endif
