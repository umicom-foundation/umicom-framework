/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/enterprise/edit_commit.h
 *
 * PURPOSE:
 *   Record a committed enterprise cell edit with optimistic revision evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ENTERPRISE_EDIT_COMMIT_H
#define UMICOM_UI_ENTERPRISE_EDIT_COMMIT_H

#include "umicom/ui/enterprise/cell_value.h"
typedef struct UmiUiEntEditCommit { uint64_t row_key; char column_id[UMI_UI_ENT_ID_CAPACITY]; UmiUiEntCellValue before_value; UmiUiEntCellValue after_value; uint64_t base_revision; uint64_t commit_revision; int committed; } UmiUiEntEditCommit;
UmiStatus umi_ui_ent_edit_commit_create(UmiUiEntEditCommit *c,uint64_t row_key,const char *column_id,const UmiUiEntCellValue *before,const UmiUiEntCellValue *after,uint64_t base_revision,uint64_t commit_revision);

#endif
