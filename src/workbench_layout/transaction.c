/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_layout/transaction.c
 *
 * PURPOSE:
 *   Implement atomic semantic layout transactions with optimistic revision checks, validation, commit evidence and deterministic rollback.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/


#include "umicom/workbench_layout/transaction.h"

#include <string.h>

#include "internal.h"

/*
 * Initialise transaction result from caller-provided values so later operations receive a
 * known state.
 */
static void transaction_result_init(
    UmiWorkbenchLayoutTransactionResult *result,
    UmiStatus status,
    UmiWorkbenchLayoutTransactionState state,
    const char *message)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (result == NULL) {
        return;
    }

    (void)memset(result, 0, sizeof(*result));
    result->structure_size = sizeof(*result);
    result->status = status;
    result->state = state;
    result->failed_operation_index = UMI_WORKBENCH_LAYOUT_INDEX_NONE;
    umi_workbench_layout_validation_report_init(&result->validation);
    (void)umi_workbench_layout_copy_text(
        result->message,
        sizeof(result->message),
        message != NULL ? message : "",
        true);
}

/*
 * Initialise workbench layout transaction from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_layout_transaction_init(
    UmiWorkbenchLayoutTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) {
        return;
    }

    (void)memset(transaction, 0, sizeof(*transaction));
    transaction->structure_size = sizeof(*transaction);
    transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_EMPTY;
    transaction->validate_on_commit = true;
}

/*
 * Provide the workbench layout transaction begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_transaction_begin(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutDocument *document,
    const char *transaction_id,
    const char *actor_id,
    const char *correlation_id,
    uint64_t expected_revision)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || document == NULL ||
        !umi_workbench_layout_text_present(transaction_id) ||
        !umi_workbench_layout_text_present(actor_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state == UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        return UMI_STATUS_BUSY;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (expected_revision != 0U &&
        document->version.revision != expected_revision) {
        return UMI_STATUS_BUSY;
    }

    umi_workbench_layout_transaction_init(transaction);
    status = umi_workbench_layout_copy_text(
        transaction->transaction_id,
        sizeof(transaction->transaction_id),
        transaction_id,
        false);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            transaction->actor_id,
            sizeof(transaction->actor_id),
            actor_id,
            false);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            transaction->correlation_id,
            sizeof(transaction->correlation_id),
            correlation_id != NULL ? correlation_id : "",
            true);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &transaction->before, document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &transaction->working, document);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        umi_workbench_layout_transaction_init(transaction);
        return status;
    }

    transaction->expected_revision = expected_revision;
    transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN;
    return UMI_STATUS_OK;
}

/*
 * Add workbench layout transaction only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_workbench_layout_transaction_append(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (transaction->operation_count >=
        UMI_WORKBENCH_LAYOUT_MAX_OPERATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_workbench_layout_operation_validate(operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    transaction->operations[transaction->operation_count] = *operation;
    transaction->operation_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Perform workbench layout transaction through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_workbench_layout_transaction_apply(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_transaction_append(
        transaction, operation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_workbench_layout_operation_apply(
        &transaction->working, operation, out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        transaction->operation_count -= 1U;
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout transaction commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_transaction_commit(
    UmiWorkbenchLayoutTransaction *transaction,
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutValidationOptions *validation_options,
    UmiWorkbenchLayoutTransactionResult *out_result)
{
    UmiWorkbenchLayoutValidationOptions effective_options;
    UmiWorkbenchLayoutValidationReport validation;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        transaction_result_init(
            out_result,
            UMI_STATUS_INVALID_STATE,
            transaction->state,
            "The transaction is not open.");
        return UMI_STATUS_INVALID_STATE;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->expected_revision != 0U &&
        destination->version.revision != transaction->expected_revision) {
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        transaction_result_init(
            out_result,
            UMI_STATUS_BUSY,
            transaction->state,
            "The destination revision changed while the transaction was open.");
        return UMI_STATUS_BUSY;
    }

    effective_options = validation_options != NULL
        ? *validation_options
        : umi_workbench_layout_validation_options_default();

    umi_workbench_layout_validation_report_init(&validation);
    status = UMI_STATUS_OK;
    /* Apply this operation only while the related capability or state is available. */
    if (transaction->validate_on_commit) {
        status = umi_workbench_layout_validate(
            &transaction->working,
            &effective_options,
            &validation);
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK || !validation.valid) {
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        transaction_result_init(
            out_result,
            status != UMI_STATUS_OK
                ? status
                : UMI_STATUS_INVALID_STATE,
            transaction->state,
            "Layout validation rejected the transaction.");
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (out_result != NULL) {
            out_result->validation = validation;
            out_result->applied_operation_count =
                transaction->operation_count;
        }
        return status != UMI_STATUS_OK
            ? status
            : UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_document_copy(
        destination, &transaction->working);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        transaction_result_init(
            out_result,
            status,
            transaction->state,
            "The committed document could not be copied.");
        return status;
    }

    transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_COMMITTED;
    transaction_result_init(
        out_result,
        UMI_STATUS_OK,
        transaction->state,
        "The layout transaction committed successfully.");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) {
        out_result->validation = validation;
        out_result->applied_operation_count =
            transaction->operation_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout transaction rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_layout_transaction_rollback(
    UmiWorkbenchLayoutTransaction *transaction,
    UmiWorkbenchLayoutTransactionResult *out_result)
{
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN &&
        transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_document_copy(
        &transaction->working, &transaction->before);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) {
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        transaction_result_init(
            out_result,
            status,
            transaction->state,
            "The transaction rollback copy failed.");
        return status;
    }

    transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_ROLLED_BACK;
    transaction_result_init(
        out_result,
        UMI_STATUS_OK,
        transaction->state,
        "The layout transaction was rolled back.");
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_result != NULL) {
        out_result->applied_operation_count =
            transaction->operation_count;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the workbench layout transaction preview operation used by this module and its
 * client applications.
 */
const UmiWorkbenchLayoutDocument *
umi_workbench_layout_transaction_preview(
    const UmiWorkbenchLayoutTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL ||
        transaction->state == UMI_WORKBENCH_LAYOUT_TRANSACTION_EMPTY) {
        return NULL;
    }
    return &transaction->working;
}
