/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/work_queue.c
 *
 * PURPOSE:
 *   Build and access the bounded execution queue without duplicating product definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/work_queue.h"
#include <string.h>
/*
 * Initialise product execution work queue from caller-provided values so later operations
 * receive a known state.
 */
void umi_product_execution_work_queue_init(UmiProductExecutionWorkQueue *queue)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue != NULL) (void)memset(queue, 0, sizeof(*queue));
}
/*
 * Provide the product execution work queue from plan operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_work_queue_from_plan(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductisationCompletionPlan *plan,
    unsigned maximum_attempts)
{
    size_t i;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || plan == NULL || maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (plan->step_count > UMI_PRODUCT_EXECUTION_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_product_execution_work_queue_init(queue);
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < plan->step_count; ++i) {
        status = umi_product_execution_work_item_init(
            &queue->items[i], i, &plan->steps[i], maximum_attempts);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    queue->count = plan->step_count;
    return UMI_STATUS_OK;
}
/*
 * Find product execution work queue while leaving the underlying catalogue or model owned
 * by this module.
 */
UmiProductExecutionWorkItem *umi_product_execution_work_queue_at(
    UmiProductExecutionWorkQueue *queue, size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || index >= queue->count) return NULL;
    return &queue->items[index];
}
/*
 * Provide the product execution work queue at const operation used by this module and its
 * client applications.
 */
const UmiProductExecutionWorkItem *umi_product_execution_work_queue_at_const(
    const UmiProductExecutionWorkQueue *queue, size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || index >= queue->count) return NULL;
    return &queue->items[index];
}
