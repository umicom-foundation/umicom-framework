/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/edit_history.h
 *
 * PURPOSE:
 *   Maintain bounded undo/redo history for committed cell edits.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_EDIT_HISTORY_H
#define UMICOM_UI_ENTERPRISE_EDIT_HISTORY_H

#include "umicom/ui/enterprise/edit_commit.h"
typedef struct UmiUiEntEditHistory { UmiUiEntEditCommit items[UMI_UI_ENT_MAX_RULES]; size_t count; size_t cursor; } UmiUiEntEditHistory;
void umi_ui_ent_edit_history_init(UmiUiEntEditHistory *h);
UmiStatus umi_ui_ent_edit_history_push(UmiUiEntEditHistory *h,const UmiUiEntEditCommit *c);
const UmiUiEntEditCommit *umi_ui_ent_edit_history_undo(UmiUiEntEditHistory *h);
const UmiUiEntEditCommit *umi_ui_ent_edit_history_redo(UmiUiEntEditHistory *h);

#endif
