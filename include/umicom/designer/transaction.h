/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/transaction.h
 *
 * PURPOSE:
 *   Group visual-builder edits into atomic, bounded undo and redo transactions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * A transaction lets several semantic changes appear as one action to the user.
 * The engine rolls back earlier changes if any later operation cannot be applied.
 */
#ifndef UMICOM_DESIGNER_TRANSACTION_H
#define UMICOM_DESIGNER_TRANSACTION_H

#include "umicom/designer/builder_types.h"
#include "umicom/designer/operation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_MAX_TRANSACTION_OPERATIONS 32U
#define UMI_DESIGNER_MAX_TRANSACTIONS 128U

/**
 * Represent the designer transaction data shared with callers of this public contract.
 */
typedef struct UmiDesignerTransaction {
    char transaction_id[UMI_DECL_ID_CAPACITY];
    char summary[UMI_DECL_TEXT_CAPACITY];
    UmiDesignerOperation operations[UMI_DESIGNER_MAX_TRANSACTION_OPERATIONS];
    size_t operation_count;
    UmiDesignerTransactionState state;
} UmiDesignerTransaction;

/**
 * Represent the designer transaction history data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerTransactionHistory UmiDesignerTransactionHistory;

/**
 * Initialise designer transaction from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_designer_transaction_init(UmiDesignerTransaction *transaction,
                                            const char *transaction_id,
                                            const char *summary);
/**
 * Add designer transaction only after its inputs and available capacity have been checked.
 */
UmiStatus umi_designer_transaction_add(UmiDesignerTransaction *transaction,
                                           const UmiDesignerOperation *operation);
/**
 * Initialise designer transaction history from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_designer_transaction_history_create(UmiDesignerDocument *document,
                                                      UmiDesignerTransactionHistory **out_history);
/**
 * Release or reset state held by designer transaction history so the same storage can be
 * reused safely.
 */
void umi_designer_transaction_history_destroy(UmiDesignerTransactionHistory *history);
/**
 * Perform designer transaction history through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_designer_transaction_history_execute(UmiDesignerTransactionHistory *history,
                                                       const UmiDesignerTransaction *transaction);
/**
 * Provide the designer transaction history undo operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_transaction_history_undo(UmiDesignerTransactionHistory *history);
/**
 * Provide the designer transaction history redo operation used by this module and its
 * client applications.
 */
UmiStatus umi_designer_transaction_history_redo(UmiDesignerTransactionHistory *history);
/**
 * Return the number of records represented by designer transaction history undo without
 * changing their state.
 */
size_t umi_designer_transaction_history_undo_count(const UmiDesignerTransactionHistory *history);
/**
 * Return the number of records represented by designer transaction history redo without
 * changing their state.
 */
size_t umi_designer_transaction_history_redo_count(const UmiDesignerTransactionHistory *history);

#ifdef __cplusplus
}
#endif
#endif
