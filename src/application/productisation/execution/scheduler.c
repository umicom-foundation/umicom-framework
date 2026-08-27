/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/scheduler.c
 *
 * PURPOSE:
 *   Refresh ready states and prioritise blocker/high-severity Framework work first.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/scheduler.h"
UmiStatus umi_product_execution_scheduler_refresh_ready(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionPolicy *policy)
{
    size_t i;
    if (queue == NULL || graph == NULL || policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    for (i = 0U; i < queue->count; ++i) {
        if (queue->items[i].state == UMI_PRODUCT_EXECUTION_PENDING ||
            (queue->items[i].state == UMI_PRODUCT_EXECUTION_FAILED &&
             umi_product_execution_work_item_retryable(&queue->items[i]))) {
            if (umi_product_execution_dependencies_satisfied(graph, queue, i) &&
                umi_product_execution_policy_check_item(policy, queue, i) == UMI_STATUS_OK) {
                (void)umi_product_execution_work_item_transition(
                    &queue->items[i], UMI_PRODUCT_EXECUTION_READY);
            }
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_product_execution_scheduler_next(
    UmiProductExecutionWorkQueue *queue,
    size_t *out_index)
{
    size_t i;
    size_t best = (size_t)-1;
    if (queue == NULL || out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (i = 0U; i < queue->count; ++i) {
        if (queue->items[i].state != UMI_PRODUCT_EXECUTION_READY) continue;
        if (best == (size_t)-1 ||
            queue->items[i].step.stage < queue->items[best].step.stage ||
            (queue->items[i].step.stage == queue->items[best].step.stage &&
             queue->items[i].step.severity > queue->items[best].step.severity) ||
            (queue->items[i].step.stage == queue->items[best].step.stage &&
             queue->items[i].step.severity == queue->items[best].step.severity &&
             queue->items[i].plan_index < queue->items[best].plan_index)) {
            best = i;
        }
    }
    if (best == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_index = best;
    return UMI_STATUS_OK;
}
