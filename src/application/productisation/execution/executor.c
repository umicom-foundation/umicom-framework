/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/productisation/execution/executor.c
 *
 * PURPOSE:
 *   Implement one governed completion execution attempt and record its outcome.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/productisation/execution/executor.h"
#include <stdio.h>
/*
 * Provide the product execution execute item operation used by this module and its client
 * applications.
 */
UmiStatus umi_product_execution_execute_item(
    UmiProductExecutionWorkQueue *queue,
    size_t item_index,
    const UmiProductExecutionPolicy *policy,
    const UmiProductExecutionAdapter *adapter,
    const UmiProductExecutionCancellation *cancellation,
    UmiProductExecutionHistory *history)
{
    UmiProductExecutionWorkItem *item;
    UmiStatus status;
    UmiProductExecutionOutcome outcome = UMI_PRODUCT_EXECUTION_OUTCOME_NONE;
    char evidence[UMI_PRODUCTISATION_REFERENCE_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (queue == NULL || policy == NULL || adapter == NULL ||
        cancellation == NULL || history == NULL || item_index >= queue->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = &queue->items[item_index];
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (item->state != UMI_PRODUCT_EXECUTION_READY) return UMI_STATUS_INVALID_STATE;
    /* Apply this branch only when its contract condition is satisfied. */
    if (umi_product_execution_cancellation_requested(cancellation)) {
        item->outcome = UMI_PRODUCT_EXECUTION_OUTCOME_CANCELLED;
        (void)umi_product_execution_work_item_transition(
            item, UMI_PRODUCT_EXECUTION_CANCELLED);
        (void)umi_product_execution_history_append(
            history, item_index, UMI_PRODUCT_EXECUTION_EVENT_CANCELLED,
            UMI_STATUS_CANCELLED, "Cancellation requested");
        return UMI_STATUS_CANCELLED;
    }
    status = umi_product_execution_policy_check_item(policy, queue, item_index);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)umi_product_execution_work_item_transition(
        item, UMI_PRODUCT_EXECUTION_RUNNING);
    item->attempts++;
    (void)umi_product_execution_history_append(
        history, item_index, UMI_PRODUCT_EXECUTION_EVENT_STARTED,
        UMI_STATUS_OK, item->step.action);
    /* Apply this branch only when its contract condition is satisfied. */
    if (policy->dry_run) {
        outcome = UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS;
        (void)snprintf(evidence, sizeof(evidence), "dry-run:%zu", item_index);
        status = UMI_STATUS_OK;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        evidence[0] = '\0';
        status = umi_product_execution_adapter_invoke(
            adapter, item, evidence, sizeof(evidence), &outcome);
    }
    item->outcome = outcome;
    (void)snprintf(item->evidence_reference, sizeof(item->evidence_reference),
                   "%s", evidence);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK && outcome == UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS) {
        (void)umi_product_execution_work_item_transition(
            item, UMI_PRODUCT_EXECUTION_SUCCEEDED);
        (void)umi_product_execution_history_append(
            history, item_index, UMI_PRODUCT_EXECUTION_EVENT_SUCCEEDED,
            UMI_STATUS_OK, evidence);
        return UMI_STATUS_OK;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (outcome == UMI_PRODUCT_EXECUTION_OUTCOME_BLOCKED) {
        (void)umi_product_execution_work_item_transition(
            item, UMI_PRODUCT_EXECUTION_BLOCKED);
        (void)umi_product_execution_history_append(
            history, item_index, UMI_PRODUCT_EXECUTION_EVENT_BLOCKED,
            status, evidence);
        return status == UMI_STATUS_OK ? UMI_STATUS_BUSY : status;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (outcome == UMI_PRODUCT_EXECUTION_OUTCOME_CANCELLED) {
        (void)umi_product_execution_work_item_transition(
            item, UMI_PRODUCT_EXECUTION_CANCELLED);
        return UMI_STATUS_CANCELLED;
    }
    (void)umi_product_execution_work_item_transition(
        item, UMI_PRODUCT_EXECUTION_FAILED);
    (void)umi_product_execution_history_append(
        history, item_index, UMI_PRODUCT_EXECUTION_EVENT_FAILED,
        status, evidence);
    return status == UMI_STATUS_OK ? UMI_STATUS_INTERNAL_ERROR : status;
}
