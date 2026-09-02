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
/**
 * Represent the ui ent edit history data shared with callers of this public contract.
 */
typedef struct UmiUiEntEditHistory { UmiUiEntEditCommit items[UMI_UI_ENT_MAX_RULES]; size_t count; size_t cursor; } UmiUiEntEditHistory;
/**
 * Initialise ui ent edit history from caller-provided values so later operations receive a
 * known state.
 */
void umi_ui_ent_edit_history_init(UmiUiEntEditHistory *h);
/**
 * Provide the ui ent edit history push operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_edit_history_push(UmiUiEntEditHistory *h,const UmiUiEntEditCommit *c);
/**
 * Provide the ui ent edit history undo operation used by this module and its client
 * applications.
 */
const UmiUiEntEditCommit *umi_ui_ent_edit_history_undo(UmiUiEntEditHistory *h);
/**
 * Provide the ui ent edit history redo operation used by this module and its client
 * applications.
 */
const UmiUiEntEditCommit *umi_ui_ent_edit_history_redo(UmiUiEntEditHistory *h);

#endif
