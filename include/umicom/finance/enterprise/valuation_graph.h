/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/finance/enterprise/valuation_graph.h
 *
 * PURPOSE:
 *   Maintain an acyclic dependency graph for valuation task execution.
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

#ifndef INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_GRAPH_H
#define INCLUDE_UMICOM_FINANCE_ENTERPRISE_VALUATION_GRAPH_H

#include "umicom/finance/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/finance/enterprise/valuation_task.h"
#include "umicom/finance/enterprise/valuation_dependency.h"
typedef struct UmiEnterpriseValuationGraph { UmiEnterpriseValuationTask tasks[64U]; size_t task_count; UmiEnterpriseValuationDependency dependencies[128U]; size_t dependency_count; } UmiEnterpriseValuationGraph;
/* Initialise an empty valuation graph. */
void umi_enterprise_valuation_graph_init(UmiEnterpriseValuationGraph *graph);
/* Add a unique valuation task. */
UmiStatus umi_enterprise_valuation_graph_add_task(UmiEnterpriseValuationGraph *graph,const UmiEnterpriseValuationTask *task);
/* Add a dependency if it preserves graph acyclicity. */
UmiStatus umi_enterprise_valuation_graph_add_dependency(UmiEnterpriseValuationGraph *graph,const UmiEnterpriseValuationDependency *dependency);

#ifdef __cplusplus
}
#endif

#endif
