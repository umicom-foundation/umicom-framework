/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/migration_graph.h
 *
 * PURPOSE:
 *   Model migration prerequisites as a bounded DAG and detect dependency cycles before execution.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_MIGRATION_GRAPH_H
#define UMICOM_DATA_ENTERPRISE_MIGRATION_GRAPH_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/migration_step.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataMigrationEdge { char from_step[UMI_DATA_ENTERPRISE_ID_CAPACITY]; char to_step[UMI_DATA_ENTERPRISE_ID_CAPACITY]; } UmiDataMigrationEdge;
typedef struct UmiDataMigrationGraph { UmiDataMigrationStep steps[UMI_DATA_ENTERPRISE_MAX_ITEMS]; size_t step_count; UmiDataMigrationEdge edges[UMI_DATA_ENTERPRISE_MAX_EDGES]; size_t edge_count; } UmiDataMigrationGraph;
/* Reset a migration dependency graph. */ void umi_data_migration_graph_init(UmiDataMigrationGraph *graph);
/* Add a unique migration step. */ UmiStatus umi_data_migration_graph_add_step(UmiDataMigrationGraph *graph,const UmiDataMigrationStep *step);
/* Add a prerequisite edge from one known step to another. */ UmiStatus umi_data_migration_graph_add_edge(UmiDataMigrationGraph *graph,const char *from_step,const char *to_step);
/* Detect dependency cycles using bounded depth-first traversal. */ bool umi_data_migration_graph_has_cycle(const UmiDataMigrationGraph *graph);

#ifdef __cplusplus
}
#endif
#endif
