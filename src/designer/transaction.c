/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/transaction.c
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

#include "umicom/designer/transaction.h"

#include <stdlib.h>
#include <string.h>

struct UmiDesignerTransactionHistory {
    UmiDesignerDocument *document;
    UmiDesignerTransaction transactions[UMI_DESIGNER_MAX_TRANSACTIONS];
    size_t count;
    size_t cursor;
};

/*
 * Initialise designer transaction from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_designer_transaction_init(UmiDesignerTransaction *transaction,
                                            const char *transaction_id,
                                            const char *summary)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || transaction_id == NULL || transaction_id[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(transaction, 0, sizeof(*transaction));
    status = umi_decl_copy_text(transaction->transaction_id, sizeof(transaction->transaction_id), transaction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_decl_copy_text(transaction->summary, sizeof(transaction->summary), summary != NULL ? summary : transaction_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    transaction->state = UMI_DESIGNER_TRANSACTION_DRAFT;
    return UMI_STATUS_OK;
}

/* Add designer transaction only after its inputs and available capacity have been checked. */
UmiStatus umi_designer_transaction_add(UmiDesignerTransaction *transaction,
                                           const UmiDesignerOperation *operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || operation == NULL || transaction->state != UMI_DESIGNER_TRANSACTION_DRAFT) {
        return UMI_STATUS_INVALID_STATE;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (transaction->operation_count >= UMI_DESIGNER_MAX_TRANSACTION_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    transaction->operations[transaction->operation_count++] = *operation;
    return UMI_STATUS_OK;
}

/*
 * Initialise designer transaction history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_transaction_history_create(UmiDesignerDocument *document,
                                                      UmiDesignerTransactionHistory **out_history)
{
    UmiDesignerTransactionHistory *history;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (document == NULL || out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;
    history = (UmiDesignerTransactionHistory *)calloc(1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;
    history->document = document;
    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by designer transaction history so the same storage can be
 * reused safely.
 */
void umi_designer_transaction_history_destroy(UmiDesignerTransactionHistory *history)
{
    free(history);
}

/* Provide the apply forward operation used by this module and its client applications. */
static UmiStatus apply_forward(UmiDesignerTransactionHistory *history, const UmiDesignerTransaction *transaction)
{
    size_t applied = 0U;
    UmiStatus status;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (applied < transaction->operation_count) {
        status = umi_designer_operation_apply(history->document, &transaction->operations[applied], 0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) {
            /*
             * Continue only while work remains available; the loop body advances the state on each
             * pass.
             */
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

/* Provide the apply reverse operation used by this module and its client applications. */
static UmiStatus apply_reverse(UmiDesignerTransactionHistory *history, const UmiDesignerTransaction *transaction)
{
    size_t index = transaction->operation_count;
    UmiStatus status;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (index > 0U) {
        --index;
        status = umi_designer_operation_apply(history->document, &transaction->operations[index], 1);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Perform designer transaction history through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_designer_transaction_history_execute(UmiDesignerTransactionHistory *history,
                                                       const UmiDesignerTransaction *transaction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || transaction == NULL || transaction->operation_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    status = apply_forward(history, transaction);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    history->count = history->cursor;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
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

/*
 * Provide the designer transaction history undo operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_transaction_history_undo(UmiDesignerTransactionHistory *history)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (history->cursor == 0U) return UMI_STATUS_NOT_FOUND;
    status = apply_reverse(history, &history->transactions[history->cursor - 1U]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        --history->cursor;
        history->transactions[history->cursor].state = UMI_DESIGNER_TRANSACTION_UNDONE;
    }
    return status;
}

/*
 * Provide the designer transaction history redo operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_transaction_history_redo(UmiDesignerTransactionHistory *history)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->cursor >= history->count) return UMI_STATUS_NOT_FOUND;
    status = apply_forward(history, &history->transactions[history->cursor]);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        history->transactions[history->cursor].state = UMI_DESIGNER_TRANSACTION_APPLIED;
        ++history->cursor;
    }
    return status;
}

/*
 * Return the number of records represented by designer transaction history undo without
 * changing their state.
 */
size_t umi_designer_transaction_history_undo_count(const UmiDesignerTransactionHistory *history)
{
    return history != NULL ? history->cursor : 0U;
}

/*
 * Return the number of records represented by designer transaction history redo without
 * changing their state.
 */
size_t umi_designer_transaction_history_redo_count(const UmiDesignerTransactionHistory *history)
{
    return history != NULL ? history->count - history->cursor : 0U;
}
