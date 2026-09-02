/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/compare_editor.c
 *
 * PURPOSE:
 *   Describe a side-by-side editor comparison and synchronized navigation policy.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/compare_editor.h"

#include <string.h>
/*
 * Initialise editor wb compare editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_compare_editor_init(UmiEditorWbCompareEditor *s,const char *a,const char *b){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(a)||!umi_editor_wb_id_valid(b)||strcmp(a,b)==0)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);(void)umi_editor_wb_copy_text(s->left_id,sizeof s->left_id,a);(void)umi_editor_wb_copy_text(s->right_id,sizeof s->right_id,b);s->sync_scroll=true;return UMI_STATUS_OK;}
