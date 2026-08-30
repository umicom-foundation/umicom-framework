/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_layout/test_operations_history.c
 *
 * PURPOSE:
 *   Verify auditable operations, optimistic revisions, atomic transactions and
 *   bounded undo/redo history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "test_fixture.h"

int main(void)
{
    UmiWorkbenchLayoutDocument *document =
        test_allocate_layout("layout.operations");
    UmiWorkbenchLayoutDocument *committed =
        (UmiWorkbenchLayoutDocument *)calloc(1U, sizeof(*committed));
    UmiWorkbenchLayoutOperation operation;
    UmiWorkbenchLayoutOperationResult operation_result;
    UmiWorkbenchLayoutTransaction *transaction =
        (UmiWorkbenchLayoutTransaction *)calloc(1U, sizeof(*transaction));
    UmiWorkbenchLayoutTransactionResult transaction_result;
    UmiWorkbenchLayoutHistory *history =
        (UmiWorkbenchLayoutHistory *)calloc(1U, sizeof(*history));
    UmiWorkbenchLayoutValidationOptions validation;
    uint64_t initial_revision;

    TEST_REQUIRE(
        document != NULL && committed != NULL &&
        transaction != NULL && history != NULL,
        "Test state must be allocated");
    initial_revision = document->version.revision;

    umi_workbench_layout_operation_init(
        &operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_SET_SPLIT_RATIO,
        "operation.split");
    test_copy_text(operation.node_id, sizeof(operation.node_id), "root");
    operation.number_value = 0.30;
    operation.expected_revision = initial_revision;
    operation.timestamp_ms = 1200U;
    TEST_STATUS_OK(umi_workbench_layout_operation_apply(
        document, &operation, &operation_result));
    TEST_REQUIRE(operation_result.changed, "Split operation must report change");
    TEST_REQUIRE(
        document->nodes[document->root_index].split_ratio == 0.30,
        "Split ratio must be updated");
    TEST_STATUS_OK(umi_workbench_layout_document_copy(committed, document));

    umi_workbench_layout_transaction_init(transaction);
    TEST_STATUS_OK(umi_workbench_layout_transaction_begin(
        transaction,
        document,
        "transaction.layout",
        "developer",
        "correlation.layout",
        document->version.revision));

    umi_workbench_layout_operation_init(
        &operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_RENAME_LAYOUT,
        "operation.rename");
    test_copy_text(
        operation.text_value,
        sizeof(operation.text_value),
        "Coding and Diagnostics");
    TEST_STATUS_OK(umi_workbench_layout_transaction_apply(
        transaction, &operation, &operation_result));

    umi_workbench_layout_operation_init(
        &operation,
        UMI_WORKBENCH_LAYOUT_OPERATION_SET_CONTEXT_GROUP,
        "operation.context");
    test_copy_text(operation.node_id, sizeof(operation.node_id), "editor");
    test_copy_text(
        operation.text_value,
        sizeof(operation.text_value),
        "blue.source-location");
    TEST_STATUS_OK(umi_workbench_layout_transaction_apply(
        transaction, &operation, &operation_result));

    validation = umi_workbench_layout_validation_options_default();
    TEST_STATUS_OK(umi_workbench_layout_transaction_commit(
        transaction,
        committed,
        &validation,
        &transaction_result));
    TEST_REQUIRE(
        strcmp(committed->name, "Coding and Diagnostics") == 0,
        "Transaction must commit metadata atomically");
    TEST_REQUIRE(
        strcmp(
            umi_workbench_layout_document_find_node(
                committed, "editor")->context_group_id,
            "blue.source-location") == 0,
        "Transaction must commit linked context state");

    umi_workbench_layout_history_init(history);
    TEST_STATUS_OK(umi_workbench_layout_history_seed(
        history,
        document,
        "initial",
        "Initial layout"));
    TEST_STATUS_OK(umi_workbench_layout_history_push(
        history,
        committed,
        "committed",
        "Committed transaction",
        "developer",
        "correlation.layout",
        1300U,
        true));
    TEST_REQUIRE(
        umi_workbench_layout_history_can_undo(history),
        "History must expose undo after a committed snapshot");
    TEST_STATUS_OK(umi_workbench_layout_history_undo(history, committed));
    TEST_REQUIRE(
        strcmp(committed->name, document->name) == 0,
        "Undo must restore the prior document");
    TEST_STATUS_OK(umi_workbench_layout_history_redo(history, committed));
    TEST_REQUIRE(
        strcmp(committed->name, "Coding and Diagnostics") == 0,
        "Redo must restore the committed document");

    free(history);
    free(transaction);
    free(committed);
    free(document);
    return 0;
}
