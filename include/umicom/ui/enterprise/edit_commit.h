/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/edit_commit.h
 *
 * PURPOSE:
 *   Record a committed enterprise cell edit with optimistic revision evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_EDIT_COMMIT_H
#define UMICOM_UI_ENTERPRISE_EDIT_COMMIT_H

#include "umicom/ui/enterprise/cell_value.h"
/**
 * Represent the ui ent edit commit data shared with callers of this public contract.
 */
typedef struct UmiUiEntEditCommit { uint64_t row_key; char column_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntCellValue before_value; UmiUiEntCellValue after_value; uint64_t base_revision; uint64_t commit_revision; int committed; } UmiUiEntEditCommit;
/**
 * Initialise ui ent edit commit from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_ui_ent_edit_commit_create(UmiUiEntEditCommit *c,uint64_t row_key,const char *column_id,const UmiUiEntCellValue *before,const UmiUiEntCellValue *after,uint64_t base_revision,uint64_t commit_revision);

#endif
