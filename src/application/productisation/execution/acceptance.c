/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/acceptance.c
 *
 * PURPOSE:
 *   Calculate acceptance from terminal state, blockers and delivery success.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/acceptance.h"
UmiProductExecutionAcceptance umi_product_execution_acceptance_evaluate(
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i;
    UmiProductExecutionAcceptance a = {1, 1, 1, 1, 0};
    if (queue == NULL || queue->count == 0U) {
        a.all_items_terminal = 0;
        a.all_delivery_succeeded = 0;
        return a;
    }
    for (i = 0U; i < queue->count; ++i) {
        const UmiProductExecutionWorkItem *item = &queue->items[i];
        if (!umi_product_execution_state_terminal(item->state)) a.all_items_terminal = 0;
        if (item->state == UMI_PRODUCT_EXECUTION_FAILED) a.no_failures = 0;
        if (item->state == UMI_PRODUCT_EXECUTION_BLOCKED ||
            (item->step.severity == UMI_PRODUCTISATION_SEVERITY_BLOCKER &&
             item->state != UMI_PRODUCT_EXECUTION_SUCCEEDED)) a.no_blockers = 0;
        if (item->step.stage != UMI_PRODUCTISATION_STAGE_ACCEPTANCE &&
            item->state != UMI_PRODUCT_EXECUTION_SUCCEEDED &&
            item->state != UMI_PRODUCT_EXECUTION_ROLLED_BACK) {
            a.all_delivery_succeeded = 0;
        }
    }
    a.accepted = a.all_items_terminal && a.no_failures &&
                 a.no_blockers && a.all_delivery_succeeded;
    return a;
}
