/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/dependency_graph.c
 *
 * PURPOSE:
 *   Implement dependency edges and deterministic productisation-stage barriers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/dependency_graph.h"
#include <string.h>
/*
 * Initialise product execution dependency graph from caller-provided values so later
 * operations receive a known state.
 */
void umi_product_execution_dependency_graph_init(
    UmiProductExecutionDependencyGraph *graph)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph != NULL) (void)memset(graph, 0, sizeof(*graph));
}
/*
 * Add product execution dependency only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_product_execution_dependency_add(
    UmiProductExecutionDependencyGraph *graph,
    size_t prerequisite,
    size_t dependent,
    size_t item_count)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || prerequisite >= item_count || dependent >= item_count ||
        prerequisite == dependent) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (graph->edges[i].prerequisite == prerequisite &&
            graph->edges[i].dependent == dependent) return UMI_STATUS_ALREADY_EXISTS;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (graph->count >= UMI_PRODUCT_EXECUTION_MAX_DEPENDENCIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->edges[graph->count].prerequisite = prerequisite;
    graph->edges[graph->count].dependent = dependent;
    graph->count++;
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution dependencies satisfied operation used by this module and
 * its client applications.
 */
int umi_product_execution_dependencies_satisfied(
    const UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || queue == NULL || item_index >= queue->count) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < graph->count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (graph->edges[i].dependent == item_index) {
            const UmiProductExecutionWorkItem *p =
                umi_product_execution_work_queue_at_const(
                    queue, graph->edges[i].prerequisite);
            /*
             * Protect caller-owned memory by checking that required state is available before it is
             * used.
             */
            if (p == NULL || p->state != UMI_PRODUCT_EXECUTION_SUCCEEDED) return 0;
        }
    }
    return 1;
}
/*
 * Provide the product execution dependency graph add stage barriers operation used by this
 * module and its client applications.
 */
UmiStatus umi_product_execution_dependency_graph_add_stage_barriers(
    UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i, j;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (graph == NULL || queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < queue->count; ++i) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (j = 0U; j < queue->count; ++j) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (queue->items[i].step.stage < queue->items[j].step.stage) {
                status = umi_product_execution_dependency_add(
                    graph, i, j, queue->count);
                /* Preserve the original failure result so the caller can respond to the correct cause. */
                if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS) {
                    return status;
                }
            }
        }
    }
    return UMI_STATUS_OK;
}
