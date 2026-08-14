/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/transaction.c
 *
 * PURPOSE:
 *   Apply atomic visual-builder transactions and maintain deterministic history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/designer/transaction.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerTransactionHistory {
    UmiDesignerDocument *document;
    UmiDesignerTransaction transactions[UMI_DESIGNER_MAX_TRANSACTIONS];
    size_t count;
    size_t cursor;
};

UmiStatus umi_designer_transaction_init(UmiDesignerTransaction *transaction,
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

UmiStatus umi_designer_transaction_add(UmiDesignerTransaction *transaction,
                                           const UmiDesignerOperation *operation)
{
    if (transaction == NULL || operation == NULL || transaction->state != UMI_DESIGNER_TRANSACTION_DRAFT) {
        return UMI_STATUS_INVALID_STATE;
    }
    if (transaction->operation_count >= UMI_DESIGNER_MAX_TRANSACTION_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    transaction->operations[transaction->operation_count++] = *operation;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_transaction_history_create(UmiDesignerDocument *document,
                                                      UmiDesignerTransactionHistory **out_history)
{
    UmiDesignerTransactionHistory *history;
    if (document == NULL || out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    history = (UmiDesignerTransactionHistory *)calloc(1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->document = document;
    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_designer_transaction_history_destroy(UmiDesignerTransactionHistory *history)
{
    free(history);
}

static UmiStatus apply_forward(UmiDesignerTransactionHistory *history, const UmiDesignerTransaction *transaction)
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

static UmiStatus apply_reverse(UmiDesignerTransactionHistory *history, const UmiDesignerTransaction *transaction)
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

UmiStatus umi_designer_transaction_history_execute(UmiDesignerTransactionHistory *history,
                                                       const UmiDesignerTransaction *transaction)
{
    UmiStatus status;
    if (history == NULL || transaction == NULL || transaction->operation_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = apply_forward(history, transaction);
    if (status != UMI_STATUS_OK) return status;
    history->count = history->cursor;
    if (history->count == UMI_DESIGNER_MAX_TRANSACTIONS) {
        (void)memmove(&history->transactions[0], &history->transactions[1],
                      (UMI_DESIGNER_MAX_TRANSACTIONS - 1U) * sizeof(history->transactions[0]));
        history->count -= 1U;
        history->cursor -= 1U;
    }
    history->transactions[history->count] = *transaction;
    history->transactions[history->count].state = UMI_DESIGNER_TRANSACTION_APPLIED;
    history->count += 1U;
    history->cursor = history->count;
    return UMI_STATUS_OK;
}

UmiStatus umi_designer_transaction_history_undo(UmiDesignerTransactionHistory *history)
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

UmiStatus umi_designer_transaction_history_redo(UmiDesignerTransactionHistory *history)
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

size_t umi_designer_transaction_history_undo_count(const UmiDesignerTransactionHistory *history)
{
    return history != NULL ? history->cursor : 0U;
}

size_t umi_designer_transaction_history_redo_count(const UmiDesignerTransactionHistory *history)
{
    return history != NULL ? history->count - history->cursor : 0U;
}
