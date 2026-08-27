/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/work_queue.c
 *
 * PURPOSE:
 *   Build and access the bounded execution queue without duplicating product definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/work_queue.h"
#include <string.h>
void umi_product_execution_work_queue_init(UmiProductExecutionWorkQueue *queue)
{
    if (queue != NULL) (void)memset(queue, 0, sizeof(*queue));
}
UmiStatus umi_product_execution_work_queue_from_plan(
    UmiProductExecutionWorkQueue *queue,
    const UmiProductisationCompletionPlan *plan,
    unsigned maximum_attempts)
{
    size_t i;
    UmiStatus status;
    if (queue == NULL || plan == NULL || maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (plan->step_count > UMI_PRODUCT_EXECUTION_MAX_ITEMS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    umi_product_execution_work_queue_init(queue);
    for (i = 0U; i < plan->step_count; ++i) {
        status = umi_product_execution_work_item_init(
            &queue->items[i], i, &plan->steps[i], maximum_attempts);
        if (status != UMI_STATUS_OK) return status;
    }
    queue->count = plan->step_count;
    return UMI_STATUS_OK;
}
UmiProductExecutionWorkItem *umi_product_execution_work_queue_at(
    UmiProductExecutionWorkQueue *queue, size_t index)
{
    if (queue == NULL || index >= queue->count) return NULL;
    return &queue->items[index];
}
const UmiProductExecutionWorkItem *umi_product_execution_work_queue_at_const(
    const UmiProductExecutionWorkQueue *queue, size_t index)
{
    if (queue == NULL || index >= queue->count) return NULL;
    return &queue->items[index];
}
