/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/active_editor.c
 *
 * PURPOSE:
 *   Track the active editor item and group as one coherent state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/active_editor.h"

#include <string.h>
/*
 * Copy editor wb active editor into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_editor_wb_active_editor_set(UmiEditorWbActiveEditor *s,const char *item,const char *group){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(item)||!umi_editor_wb_id_valid(group))return UMI_STATUS_INVALID_ARGUMENT;(void)umi_editor_wb_copy_text(s->item_id,sizeof s->item_id,item);(void)umi_editor_wb_copy_text(s->group_id,sizeof s->group_id,group);s->revision++;return UMI_STATUS_OK;} void umi_editor_wb_active_editor_clear(UmiEditorWbActiveEditor *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){s->item_id[0]='\0';s->group_id[0]='\0';s->revision++;}}
