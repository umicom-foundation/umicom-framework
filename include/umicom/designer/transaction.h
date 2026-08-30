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

/* BEGINNER NOTE:
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

typedef struct UmiDesignerTransaction {
    char transaction_id[UMI_DECL_ID_CAPACITY];
    char summary[UMI_DECL_TEXT_CAPACITY];
    UmiDesignerOperation operations[UMI_DESIGNER_MAX_TRANSACTION_OPERATIONS];
    size_t operation_count;
    UmiDesignerTransactionState state;
} UmiDesignerTransaction;

typedef struct UmiDesignerTransactionHistory UmiDesignerTransactionHistory;

UmiStatus umi_designer_transaction_init(UmiDesignerTransaction *transaction,
                                            const char *transaction_id,
                                            const char *summary);
UmiStatus umi_designer_transaction_add(UmiDesignerTransaction *transaction,
                                           const UmiDesignerOperation *operation);
UmiStatus umi_designer_transaction_history_create(UmiDesignerDocument *document,
                                                      UmiDesignerTransactionHistory **out_history);
void umi_designer_transaction_history_destroy(UmiDesignerTransactionHistory *history);
UmiStatus umi_designer_transaction_history_execute(UmiDesignerTransactionHistory *history,
                                                       const UmiDesignerTransaction *transaction);
UmiStatus umi_designer_transaction_history_undo(UmiDesignerTransactionHistory *history);
UmiStatus umi_designer_transaction_history_redo(UmiDesignerTransactionHistory *history);
size_t umi_designer_transaction_history_undo_count(const UmiDesignerTransactionHistory *history);
size_t umi_designer_transaction_history_redo_count(const UmiDesignerTransactionHistory *history);

#ifdef __cplusplus
}
#endif
#endif
