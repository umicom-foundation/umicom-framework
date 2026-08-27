/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/work_item.c
 *
 * PURPOSE:
 *   Initialise completion work items and enforce their lifecycle transitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/work_item.h"
#include <string.h>

UmiStatus umi_product_execution_work_item_init(
    UmiProductExecutionWorkItem *item,
    size_t plan_index,
    const UmiProductisationCompletionStep *step,
    unsigned maximum_attempts)
{
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

UmiStatus umi_product_execution_work_item_transition(
    UmiProductExecutionWorkItem *item,
    UmiProductExecutionState next)
{
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_product_execution_state_can_transition(item->state, next)) {
        return UMI_STATUS_INVALID_STATE;
    }
    item->state = next;
    return UMI_STATUS_OK;
}

int umi_product_execution_work_item_retryable(
    const UmiProductExecutionWorkItem *item)
{
    return item != NULL &&
           item->outcome == UMI_PRODUCT_EXECUTION_OUTCOME_RETRYABLE_FAILURE &&
           item->attempts < item->maximum_attempts;
}
