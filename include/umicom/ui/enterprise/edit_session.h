/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/edit_session.h
 *
 * PURPOSE:
 *   Track non-destructive cell editing before a backend commit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_EDIT_SESSION_H
#define UMICOM_UI_ENTERPRISE_EDIT_SESSION_H

#include "umicom/ui/enterprise/cell_value.h"
/**
 * Represent the ui ent edit session data shared with callers of this public contract.
 */
typedef struct UmiUiEntEditSession { uint64_t row_key; char column_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntCellValue original_value; UmiUiEntCellValue pending_value; int active; int dirty; uint64_t revision; } UmiUiEntEditSession;
/**
 * Provide the ui ent edit session begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_edit_session_begin(UmiUiEntEditSession *s,uint64_t row_key,const char *column_id,const UmiUiEntCellValue *value);
/**
 * Provide the ui ent edit session update operation used by this module and its client
 * applications.
 */
UmiStatus umi_ui_ent_edit_session_update(UmiUiEntEditSession *s,const UmiUiEntCellValue *value);
/**
 * Provide the ui ent edit session cancel operation used by this module and its client
 * applications.
 */
void umi_ui_ent_edit_session_cancel(UmiUiEntEditSession *s);

#endif
