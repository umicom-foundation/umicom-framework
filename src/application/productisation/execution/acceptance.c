/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/acceptance.c
 *
 * PURPOSE:
 *   Calculate acceptance from terminal state, blockers and delivery success.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/acceptance.h"
/*
 * Provide the product execution acceptance evaluate operation used by this module and its
 * client applications.
 */
UmiProductExecutionAcceptance umi_product_execution_acceptance_evaluate(
    const UmiProductExecutionWorkQueue *queue)
{
    size_t i;
    UmiProductExecutionAcceptance a = {1, 1, 1, 1, 0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || queue->count == 0U) {
        a.all_items_terminal = 0;
        a.all_delivery_succeeded = 0;
        return a;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (i = 0U; i < queue->count; ++i) {
        const UmiProductExecutionWorkItem *item = &queue->items[i];
        /* Apply this branch only when its contract condition is satisfied. */
        if (!umi_product_execution_state_terminal(item->state)) a.all_items_terminal = 0;
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (item->state == UMI_PRODUCT_EXECUTION_FAILED) a.no_failures = 0;
        /* Apply this branch only when its contract condition is satisfied. */
        if (item->state == UMI_PRODUCT_EXECUTION_BLOCKED ||
            (item->step.severity == UMI_PRODUCTISATION_SEVERITY_BLOCKER &&
             item->state != UMI_PRODUCT_EXECUTION_SUCCEEDED)) a.no_blockers = 0;
        /* Apply this branch only when its contract condition is satisfied. */
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
