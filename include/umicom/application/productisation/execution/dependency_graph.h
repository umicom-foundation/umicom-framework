/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/application/productisation/execution/dependency_graph.h
 *
 * PURPOSE:
 *   Track explicit completion-step prerequisites while preserving Framework-first ordering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_DEPENDENCY_GRAPH_H
#define UMICOM_APPLICATION_PRODUCTISATION_EXECUTION_DEPENDENCY_GRAPH_H
#include "umicom/application/productisation/execution/work_queue.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiProductExecutionDependency {
    size_t prerequisite;
    size_t dependent;
} UmiProductExecutionDependency;
typedef struct UmiProductExecutionDependencyGraph {
    UmiProductExecutionDependency edges[UMI_PRODUCT_EXECUTION_MAX_DEPENDENCIES];
    size_t count;
} UmiProductExecutionDependencyGraph;
/** Clear all dependency edges before a plan is loaded. */
void umi_product_execution_dependency_graph_init(
    UmiProductExecutionDependencyGraph *graph);
/** Add one prerequisite edge while rejecting duplicates and cycles. */
UmiStatus umi_product_execution_dependency_add(
    UmiProductExecutionDependencyGraph *graph,
    size_t prerequisite,
    size_t dependent,
    size_t item_count);
/** Return non-zero when every prerequisite item completed successfully. */
int umi_product_execution_dependencies_satisfied(
    const UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index);
/** Add ordering barriers between Framework, app, adapter and acceptance stages. */
UmiStatus umi_product_execution_dependency_graph_add_stage_barriers(
    UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionWorkQueue *queue);
#ifdef __cplusplus
}
#endif
#endif
