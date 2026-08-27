/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/rollback.c
 *
 * PURPOSE:
 *   Build and advance reverse-order compensation plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/rollback.h"
#include <string.h>
void umi_product_execution_rollback_plan_init(UmiProductExecutionRollbackPlan *plan)
{
    if (plan != NULL) (void)memset(plan, 0, sizeof(*plan));
}
UmiStatus umi_product_execution_rollback_plan_build(
    UmiProductExecutionRollbackPlan *plan,
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i;
    if (plan == NULL || queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_product_execution_rollback_plan_init(plan);
    for (i = queue->count; i > 0U; --i) {
        if (queue->items[i - 1U].state == UMI_PRODUCT_EXECUTION_SUCCEEDED) {
            if (plan->count >= UMI_PRODUCT_EXECUTION_MAX_ROLLBACKS)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            plan->entries[plan->count++].item_index = i - 1U;
        }
    }
    return UMI_STATUS_OK;
}
UmiStatus umi_product_execution_rollback_next(
    UmiProductExecutionRollbackPlan *plan,
    size_t *out_item_index)
{
    if (plan == NULL || out_item_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan->cursor >= plan->count) return UMI_STATUS_NOT_FOUND;
    *out_item_index = plan->entries[plan->cursor].item_index;
    return UMI_STATUS_OK;
}
UmiStatus umi_product_execution_rollback_record(
    UmiProductExecutionRollbackPlan *plan,
    int succeeded)
{
    if (plan == NULL || plan->cursor >= plan->count) return UMI_STATUS_INVALID_STATE;
    if (!succeeded) return UMI_STATUS_INTERNAL_ERROR;
    plan->entries[plan->cursor].completed = 1;
    plan->cursor++;
    return UMI_STATUS_OK;
}
