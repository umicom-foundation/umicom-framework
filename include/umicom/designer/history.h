/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/history.h
 *
 * PURPOSE:
 *   Maintain bounded undo and redo history for semantic designer operations rather than toolkit widget mutations.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This public contract describes reusable visual-designer state and operations.
 * Product-specific windows remain outside the Framework engine.
 */

#ifndef UMICOM_DESIGNER_HISTORY_H
#define UMICOM_DESIGNER_HISTORY_H
#include "umicom/designer/operation.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef struct UmiDesignerHistory UmiDesignerHistory;
UmiStatus umi_designer_history_create(UmiDesignerDocument *document,UmiDesignerHistory **out_history);
void umi_designer_history_destroy(UmiDesignerHistory *history);
UmiStatus umi_designer_history_execute(UmiDesignerHistory *history,const UmiDesignerOperation *operation);
UmiStatus umi_designer_history_undo(UmiDesignerHistory *history);
UmiStatus umi_designer_history_redo(UmiDesignerHistory *history);
size_t umi_designer_history_undo_count(const UmiDesignerHistory *history);
size_t umi_designer_history_redo_count(const UmiDesignerHistory *history);
#ifdef __cplusplus
}
#endif
#endif
