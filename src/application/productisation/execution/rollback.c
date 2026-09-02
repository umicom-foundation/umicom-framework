/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/rollback.c
 *
 * PURPOSE:
 *   Build and advance reverse-order compensation plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/rollback.h"
#include <string.h>
/*
 * Initialise product execution rollback plan from caller-provided values so later
 * operations receive a known state.
 */
void umi_product_execution_rollback_plan_init(UmiProductExecutionRollbackPlan *plan)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan != NULL) (void)memset(plan, 0, sizeof(*plan));
}
/*
 * Provide the product execution rollback plan build operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_rollback_plan_build(
    UmiProductExecutionRollbackPlan *plan,
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || queue == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    umi_product_execution_rollback_plan_init(plan);
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = queue->count; i > 0U; --i) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (queue->items[i - 1U].state == UMI_PRODUCT_EXECUTION_SUCCEEDED) {
            /* Keep the operation inside its valid bounds before reading, writing or adding data. */
            if (plan->count >= UMI_PRODUCT_EXECUTION_MAX_ROLLBACKS)
                return UMI_STATUS_CAPACITY_EXCEEDED;
            plan->entries[plan->count++].item_index = i - 1U;
        }
    }
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution rollback next operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_execution_rollback_next(
    UmiProductExecutionRollbackPlan *plan,
    size_t *out_item_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || out_item_index == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan->cursor >= plan->count) return UMI_STATUS_NOT_FOUND;
    *out_item_index = plan->entries[plan->cursor].item_index;
    return UMI_STATUS_OK;
}
/*
 * Provide the product execution rollback record operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_rollback_record(
    UmiProductExecutionRollbackPlan *plan,
    int succeeded)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (plan == NULL || plan->cursor >= plan->count) return UMI_STATUS_INVALID_STATE;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!succeeded) return UMI_STATUS_INTERNAL_ERROR;
    plan->entries[plan->cursor].completed = 1;
    plan->cursor++;
    return UMI_STATUS_OK;
}
