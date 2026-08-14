/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/transaction_v2.h
 *
 * PURPOSE:
 *   Group visual-builder edits into atomic, bounded undo and redo transactions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * A transaction lets several semantic changes appear as one action to the user.
 * The engine rolls back earlier changes if any later operation cannot be applied.
 */
#ifndef UMICOM_DESIGNER_TRANSACTION_V2_H
#define UMICOM_DESIGNER_TRANSACTION_V2_H

#include "umicom/designer/builder_types_v2.h"
#include "umicom/designer/operation.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_DESIGNER_V2_MAX_TRANSACTION_OPERATIONS 32U
#define UMI_DESIGNER_V2_MAX_TRANSACTIONS 128U

typedef struct UmiDesignerTransactionV2 {
    char transaction_id[UMI_DECL_ID_CAPACITY];
    char summary[UMI_DECL_TEXT_CAPACITY];
    UmiDesignerOperation operations[UMI_DESIGNER_V2_MAX_TRANSACTION_OPERATIONS];
    size_t operation_count;
    UmiDesignerTransactionStateV2 state;
} UmiDesignerTransactionV2;

typedef struct UmiDesignerTransactionHistoryV2 UmiDesignerTransactionHistoryV2;

UmiStatus umi_designer_transaction_v2_init(UmiDesignerTransactionV2 *transaction,
                                            const char *transaction_id,
                                            const char *summary);
UmiStatus umi_designer_transaction_v2_add(UmiDesignerTransactionV2 *transaction,
                                           const UmiDesignerOperation *operation);
UmiStatus umi_designer_transaction_history_v2_create(UmiDesignerDocument *document,
                                                      UmiDesignerTransactionHistoryV2 **out_history);
void umi_designer_transaction_history_v2_destroy(UmiDesignerTransactionHistoryV2 *history);
UmiStatus umi_designer_transaction_history_v2_execute(UmiDesignerTransactionHistoryV2 *history,
                                                       const UmiDesignerTransactionV2 *transaction);
UmiStatus umi_designer_transaction_history_v2_undo(UmiDesignerTransactionHistoryV2 *history);
UmiStatus umi_designer_transaction_history_v2_redo(UmiDesignerTransactionHistoryV2 *history);
size_t umi_designer_transaction_history_v2_undo_count(const UmiDesignerTransactionHistoryV2 *history);
size_t umi_designer_transaction_history_v2_redo_count(const UmiDesignerTransactionHistoryV2 *history);

#ifdef __cplusplus
}
#endif
#endif
