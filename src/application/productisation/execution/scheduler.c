/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/scheduler.c
 *
 * PURPOSE:
 *   Refresh ready states and prioritise blocker/high-severity Framework work first.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/scheduler.h"
/*
 * Provide the product execution scheduler refresh ready operation used by this module and
 * its client applications.
 */
UmiStatus umi_product_execution_scheduler_refresh_ready(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductExecutionDependencyGraph *graph,
    const UmiProductExecutionPolicy *policy)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || graph == NULL || policy == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < queue->count; ++i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (queue->items[i].state == UMI_PRODUCT_EXECUTION_PENDING ||
            (queue->items[i].state == UMI_PRODUCT_EXECUTION_FAILED &&
             umi_product_execution_work_item_retryable(&queue->items[i]))) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (umi_product_execution_dependencies_satisfied(graph, queue, i) &&
                umi_product_execution_policy_check_item(policy, queue, i) == UMI_STATUS_OK) {
                (void)umi_product_execution_work_item_transition(
                    &queue->items[i], UMI_PRODUCT_EXECUTION_READY);
            }
        }
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution scheduler next operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_scheduler_next(
    UmiProductExecutionWorkQueue *queue,
    size_t *out_index)
{
    size_t i;
    size_t best = (size_t)-1;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || out_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < queue->count; ++i) {
        /* Apply this operation only while the related capability or state is available. */
        if (queue->items[i].state != UMI_PRODUCT_EXECUTION_READY) continue;
        /* Apply this branch only when its contract condition is satisfied. */
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
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (best == (size_t)-1) return UMI_STATUS_NOT_FOUND;
    *out_index = best;
    return UMI_STATUS_OK;
}
