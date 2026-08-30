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

static void transaction_result_init(
    UmiWorkbenchLayoutTransactionResult *result,
    UmiStatus status,
    UmiWorkbenchLayoutTransactionState state,
    const char *message)
{
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

void umi_workbench_layout_transaction_init(
    UmiWorkbenchLayoutTransaction *transaction)
{
    if (transaction == NULL) {
        return;
    }

    (void)memset(transaction, 0, sizeof(*transaction));
    transaction->structure_size = sizeof(*transaction);
    transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_EMPTY;
    transaction->validate_on_commit = true;
}

UmiStatus umi_workbench_layout_transaction_begin(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutDocument *document,
    const char *transaction_id,
    const char *actor_id,
    const char *correlation_id,
    uint64_t expected_revision)
{
    UmiStatus status;

    if (transaction == NULL || document == NULL ||
        !umi_workbench_layout_text_present(transaction_id) ||
        !umi_workbench_layout_text_present(actor_id)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (transaction->state == UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        return UMI_STATUS_BUSY;
    }
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
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            transaction->actor_id,
            sizeof(transaction->actor_id),
            actor_id,
            false);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_copy_text(
            transaction->correlation_id,
            sizeof(transaction->correlation_id),
            correlation_id != NULL ? correlation_id : "",
            true);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &transaction->before, document);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_workbench_layout_document_copy(
            &transaction->working, document);
    }
    if (status != UMI_STATUS_OK) {
        umi_workbench_layout_transaction_init(transaction);
        return status;
    }

    transaction->expected_revision = expected_revision;
    transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_transaction_append(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutOperation *operation)
{
    UmiStatus status;

    if (transaction == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (transaction->operation_count >=
        UMI_WORKBENCH_LAYOUT_MAX_OPERATIONS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }

    status = umi_workbench_layout_operation_validate(operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    transaction->operations[transaction->operation_count] = *operation;
    transaction->operation_count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_transaction_apply(
    UmiWorkbenchLayoutTransaction *transaction,
    const UmiWorkbenchLayoutOperation *operation,
    UmiWorkbenchLayoutOperationResult *out_result)
{
    UmiStatus status;

    if (transaction == NULL || operation == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_transaction_append(
        transaction, operation);
    if (status != UMI_STATUS_OK) {
        return status;
    }

    status = umi_workbench_layout_operation_apply(
        &transaction->working, operation, out_result);
    if (status != UMI_STATUS_OK) {
        transaction->operation_count -= 1U;
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_transaction_commit(
    UmiWorkbenchLayoutTransaction *transaction,
    UmiWorkbenchLayoutDocument *destination,
    const UmiWorkbenchLayoutValidationOptions *validation_options,
    UmiWorkbenchLayoutTransactionResult *out_result)
{
    UmiWorkbenchLayoutValidationOptions effective_options;
    UmiWorkbenchLayoutValidationReport validation;
    UmiStatus status;

    if (transaction == NULL || destination == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN) {
        transaction_result_init(
            out_result,
            UMI_STATUS_INVALID_STATE,
            transaction->state,
            "The transaction is not open.");
        return UMI_STATUS_INVALID_STATE;
    }
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
    if (transaction->validate_on_commit) {
        status = umi_workbench_layout_validate(
            &transaction->working,
            &effective_options,
            &validation);
    }
    if (status != UMI_STATUS_OK || !validation.valid) {
        transaction->state = UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED;
        transaction_result_init(
            out_result,
            status != UMI_STATUS_OK
                ? status
                : UMI_STATUS_INVALID_STATE,
            transaction->state,
            "Layout validation rejected the transaction.");
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
    if (out_result != NULL) {
        out_result->validation = validation;
        out_result->applied_operation_count =
            transaction->operation_count;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_workbench_layout_transaction_rollback(
    UmiWorkbenchLayoutTransaction *transaction,
    UmiWorkbenchLayoutTransactionResult *out_result)
{
    UmiStatus status;

    if (transaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_OPEN &&
        transaction->state != UMI_WORKBENCH_LAYOUT_TRANSACTION_FAILED) {
        return UMI_STATUS_INVALID_STATE;
    }

    status = umi_workbench_layout_document_copy(
        &transaction->working, &transaction->before);
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
    if (out_result != NULL) {
        out_result->applied_operation_count =
            transaction->operation_count;
    }
    return UMI_STATUS_OK;
}

const UmiWorkbenchLayoutDocument *
umi_workbench_layout_transaction_preview(
    const UmiWorkbenchLayoutTransaction *transaction)
{
    if (transaction == NULL ||
        transaction->state == UMI_WORKBENCH_LAYOUT_TRANSACTION_EMPTY) {
        return NULL;
    }
    return &transaction->working;
}
