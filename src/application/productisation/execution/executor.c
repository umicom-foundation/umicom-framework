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
    if (queue == NULL || policy == NULL || adapter == NULL ||
        cancellation == NULL || history == NULL || item_index >= queue->count) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    item = &queue->items[item_index];
    if (item->state != UMI_PRODUCT_EXECUTION_READY) return UMI_STATUS_INVALID_STATE;
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
    if (status != UMI_STATUS_OK) return status;
    (void)umi_product_execution_work_item_transition(
        item, UMI_PRODUCT_EXECUTION_RUNNING);
    item->attempts++;
    (void)umi_product_execution_history_append(
        history, item_index, UMI_PRODUCT_EXECUTION_EVENT_STARTED,
        UMI_STATUS_OK, item->step.action);
    if (policy->dry_run) {
        outcome = UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS;
        (void)snprintf(evidence, sizeof(evidence), "dry-run:%zu", item_index);
        status = UMI_STATUS_OK;
    } else {
        evidence[0] = '\0';
        status = umi_product_execution_adapter_invoke(
            adapter, item, evidence, sizeof(evidence), &outcome);
    }
    item->outcome = outcome;
    (void)snprintf(item->evidence_reference, sizeof(item->evidence_reference),
                   "%s", evidence);
    if (status == UMI_STATUS_OK && outcome == UMI_PRODUCT_EXECUTION_OUTCOME_SUCCESS) {
        (void)umi_product_execution_work_item_transition(
            item, UMI_PRODUCT_EXECUTION_SUCCEEDED);
        (void)umi_product_execution_history_append(
            history, item_index, UMI_PRODUCT_EXECUTION_EVENT_SUCCEEDED,
            UMI_STATUS_OK, evidence);
        return UMI_STATUS_OK;
    }
    if (outcome == UMI_PRODUCT_EXECUTION_OUTCOME_BLOCKED) {
        (void)umi_product_execution_work_item_transition(
            item, UMI_PRODUCT_EXECUTION_BLOCKED);
        (void)umi_product_execution_history_append(
            history, item_index, UMI_PRODUCT_EXECUTION_EVENT_BLOCKED,
            status, evidence);
        return status == UMI_STATUS_OK ? UMI_STATUS_BUSY : status;
    }
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
