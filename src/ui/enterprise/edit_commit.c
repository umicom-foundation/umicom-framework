/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/enterprise/edit_commit.c
 *
 * PURPOSE:
 *   Implement immutable edit-commit evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/enterprise/edit_commit.h"
#include <string.h>
UmiStatus umi_ui_ent_edit_commit_create(UmiUiEntEditCommit *c,uint64_t key,const char *col,const UmiUiEntCellValue *a,const UmiUiEntCellValue *b,uint64_t base,uint64_t rev){if(!c||key==0U||!umi_ui_ent_id_valid(col)||!a||!b||rev<=base)return UMI_STATUS_INVALID_ARGUMENT;memset(c,0,sizeof *c);c->row_key=key;if(umi_ui_ent_copy_text(c->column_id,sizeof c->column_id,col)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;c->before_value=*a;c->after_value=*b;c->base_revision=base;c->commit_revision=rev;c->committed=1;return UMI_STATUS_OK;}
