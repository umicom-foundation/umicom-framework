/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/work_item.c
 *
 * PURPOSE:
 *   Initialise completion work items and enforce their lifecycle transitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/work_item.h"
#include <string.h>

/*
 * Initialise product execution work item from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_product_execution_work_item_init(
    UmiProductExecutionWorkItem *item,
    size_t plan_index,
    const UmiProductisationCompletionStep *step,
    unsigned maximum_attempts)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL || step == NULL || maximum_attempts == 0U) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(item, 0, sizeof(*item));
    item->plan_index = plan_index;
    item->step = *step;
    item->state = UMI_PRODUCT_EXECUTION_PENDING;
    item->outcome = UMI_PRODUCT_EXECUTION_OUTCOME_NONE;
    item->maximum_attempts = maximum_attempts;
    return UMI_STATUS_OK;
}

/*
 * Provide the product execution work item transition operation used by this module and its
 * client applications.
 */
UmiStatus umi_product_execution_work_item_transition(
    UmiProductExecutionWorkItem *item,
    UmiProductExecutionState next)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Apply this branch only when its contract condition is satisfied. */
    if (!umi_product_execution_state_can_transition(item->state, next)) {
        return UMI_STATUS_INVALID_STATE;
    }
    item->state = next;
    return UMI_STATUS_OK;
}

/*
 * Provide the product execution work item retryable operation used by this module and its
 * client applications.
 */
int umi_product_execution_work_item_retryable(
    const UmiProductExecutionWorkItem *item)
{
    return item != NULL &&
           item->outcome == UMI_PRODUCT_EXECUTION_OUTCOME_RETRYABLE_FAILURE &&
           item->attempts < item->maximum_attempts;
}
