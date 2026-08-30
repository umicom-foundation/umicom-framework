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
void umi_product_execution_dependency_graph_init(
    UmiProductExecutionDependencyGraph *graph)
{
    if (graph != NULL) (void)memset(graph, 0, sizeof(*graph));
}
UmiStatus umi_product_execution_dependency_add(
    UmiProductExecutionDependencyGraph *graph,
    size_t prerequisite,
    size_t dependent,
    size_t item_count)
{
    size_t i;
    if (graph == NULL || prerequisite >= item_count || dependent >= item_count ||
        prerequisite == dependent) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < graph->count; ++i) {
        if (graph->edges[i].prerequisite == prerequisite &&
            graph->edges[i].dependent == dependent) return UMI_STATUS_ALREADY_EXISTS;
    }
    if (graph->count >= UMI_PRODUCT_EXECUTION_MAX_DEPENDENCIES) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    graph->edges[graph->count].prerequisite = prerequisite;
    graph->edges[graph->count].dependent = dependent;
    graph->count++;
    return UMI_STATUS_OK;
}
int umi_product_execution_dependencies_satisfied(
    const UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionWorkQueue *queue,
    size_t item_index)
{
    size_t i;
    if (graph == NULL || queue == NULL || item_index >= queue->count) return 0;
    for (i = 0U; i < graph->count; ++i) {
        if (graph->edges[i].dependent == item_index) {
            const UmiProductExecutionWorkItem *p =
                umi_product_execution_work_queue_at_const(
                    queue, graph->edges[i].prerequisite);
            if (p == NULL || p->state != UMI_PRODUCT_EXECUTION_SUCCEEDED) return 0;
        }
    }
    return 1;
}
UmiStatus umi_product_execution_dependency_graph_add_stage_barriers(
    UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i, j;
    UmiStatus status;
    if (graph == NULL || queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < queue->count; ++i) {
        for (j = 0U; j < queue->count; ++j) {
            if (queue->items[i].step.stage < queue->items[j].step.stage) {
                status = umi_product_execution_dependency_add(
                    graph, i, j, queue->count);
                if (status != UMI_STATUS_OK && status != UMI_STATUS_ALREADY_EXISTS) {
                    return status;
                }
            }
        }
    }
    return UMI_STATUS_OK;
}
