/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/transaction_v2.c
 *
 * PURPOSE:
 *   Apply atomic visual-builder transactions and maintain deterministic history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/transaction_v2.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerTransactionHistoryV2 {
    UmiDesignerDocument *document;
    UmiDesignerTransactionV2 transactions[UMI_DESIGNER_V2_MAX_TRANSACTIONS];
    size_t count;
    size_t cursor;
};

UmiStatus umi_designer_transaction_v2_init(UmiDesignerTransactionV2 *transaction,
                                            const char *transaction_id,
                                            const char *summary)
{
    UmiStatus status;
    if (transaction == NULL || transaction_id == NULL || transaction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(transaction, 0, sizeof(*transaction));
    status = umi_decl_copy_text(transaction->transaction_id, sizeof(transaction->transaction_id), transaction_id);
    if (status != UMI_STATUS_OK) return status;
    status = umi_decl_copy_text(transaction->summary, sizeof(transaction->summary), summary != NULL ? summary : transaction_id);
    if (status != UMI_STATUS_OK) return status;
    transaction->state = UMI_DESIGNER_TRANSACTION_DRAFT;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_transaction_v2_add(UmiDesignerTransactionV2 *transaction,
                                           const UmiDesignerOperation *operation)
{
    if (transaction == NULL || operation == NULL || transaction->state != UMI_DESIGNER_TRANSACTION_DRAFT) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (transaction->operation_count >= UMI_DESIGNER_V2_MAX_TRANSACTION_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    transaction->operations[transaction->operation_count++] = *operation;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_transaction_history_v2_create(UmiDesignerDocument *document,
                                                      UmiDesignerTransactionHistoryV2 **out_history)
{
    UmiDesignerTransactionHistoryV2 *history;
    if (document == NULL || out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    history = (UmiDesignerTransactionHistoryV2 *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->document = document;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_designer_transaction_history_v2_destroy(UmiDesignerTransactionHistoryV2 *history)
{
    free(history);
}

static UmiStatus apply_forward(UmiDesignerTransactionHistoryV2 *history, const UmiDesignerTransactionV2 *transaction)
{
    size_t applied = 0U;
    UmiStatus status;
    while (applied < transaction->operation_count) {
        status = umi_designer_operation_apply(history->document, &transaction->operations[applied], 0);
        if (status != UMI_STATUS_OK) {
            while (applied > 0U) {
                --applied;
                (void)umi_designer_operation_apply(history->document, &transaction->operations[applied], 1);
            }
            return status;
        }
        ++applied;
    }
    return UMI_STATUS_OK;
}

static UmiStatus apply_reverse(UmiDesignerTransactionHistoryV2 *history, const UmiDesignerTransactionV2 *transaction)
{
    size_t index = transaction->operation_count;
    UmiStatus status;
    while (index > 0U) {
        --index;
        status = umi_designer_operation_apply(history->document, &transaction->operations[index], 1);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_transaction_history_v2_execute(UmiDesignerTransactionHistoryV2 *history,
                                                       const UmiDesignerTransactionV2 *transaction)
{
    UmiDesignerTransactionV2 committed;
    UmiStatus status;
    if (history == NULL || transaction == NULL || transaction->operation_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = apply_forward(history, transaction);
    if (status != UMI_STATUS_OK) return status;
    committed = *transaction;
    committed.state = UMI_DESIGNER_TRANSACTION_APPLIED;
    history->count = history->cursor;
    if (history->count == UMI_DESIGNER_V2_MAX_TRANSACTIONS) {
        (void)memmove(&history->transactions[0], &history->transactions[1],
                      (UMI_DESIGNER_V2_MAX_TRANSACTIONS - 1U) * sizeof(history->transactions[0]));
        history->count -= 1U;
        history->cursor -= 1U;
    }
    history->transactions[history->count++] = committed;
    history->cursor = history->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_transaction_history_v2_undo(UmiDesignerTransactionHistoryV2 *history)
{
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->cursor == 0U) return UMI_STATUS_NOT_FOUND;
    status = apply_reverse(history, &history->transactions[history->cursor - 1U]);
    if (status == UMI_STATUS_OK) {
        --history->cursor;
        history->transactions[history->cursor].state = UMI_DESIGNER_TRANSACTION_UNDONE;
    }
    return status;
}

UmiStatus umi_designer_transaction_history_v2_redo(UmiDesignerTransactionHistoryV2 *history)
{
    UmiStatus status;
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (history->cursor >= history->count) return UMI_STATUS_NOT_FOUND;
    status = apply_forward(history, &history->transactions[history->cursor]);
    if (status == UMI_STATUS_OK) {
        history->transactions[history->cursor].state = UMI_DESIGNER_TRANSACTION_APPLIED;
        ++history->cursor;
    }
    return status;
}

size_t umi_designer_transaction_history_v2_undo_count(const UmiDesignerTransactionHistoryV2 *history)
{
    return history != NULL ? history->cursor : 0U;
}

size_t umi_designer_transaction_history_v2_redo_count(const UmiDesignerTransactionHistoryV2 *history)
{
    return history != NULL ? history->count - history->cursor : 0U;
}
