/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/history.h
 *
 * PURPOSE:
 *   Maintain bounded undo and redo history for semantic designer operations rather than toolkit widget mutations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_HISTORY_H
#define UMICOM_DESIGNER_HISTORY_H
#include "umicom/designer/operation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the designer history data shared with callers of this public contract.
 */
typedef struct UmiDesignerHistory UmiDesignerHistory;
/**
 * Initialise designer history from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_designer_history_create(UmiDesignerDocument *document,UmiDesignerHistory **out_history);
/**
 * Release or reset state held by designer history so the same storage can be reused
 * safely.
 */
void umi_designer_history_destroy(UmiDesignerHistory *history);
/**
 * Perform designer history through the module contract so client applications do not
 * duplicate its policy.
 */
UmiStatus umi_designer_history_execute(UmiDesignerHistory *history,const UmiDesignerOperation *operation);
/**
 * Provide the designer history undo operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_history_undo(UmiDesignerHistory *history);
/**
 * Provide the designer history redo operation used by this module and its client
 * applications.
 */
UmiStatus umi_designer_history_redo(UmiDesignerHistory *history);
/**
 * Return the number of records represented by designer history undo without changing their
 * state.
 */
size_t umi_designer_history_undo_count(const UmiDesignerHistory *history);
/**
 * Return the number of records represented by designer history redo without changing their
 * state.
 */
size_t umi_designer_history_redo_count(const UmiDesignerHistory *history);
#ifdef __cplusplus
}
#endif
#endif
