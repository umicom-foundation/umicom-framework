/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/multi_cursor_model.c
 *
 * PURPOSE:
 *   Maintain distinct editor cursor positions for multi-cursor editing.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/multi_cursor_model.h"

#include <string.h>
void umi_editor_wb_multi_cursor_model_init(UmiEditorWbMultiCursorModel *m){if(m!=NULL)memset(m,0,sizeof *m);}
UmiStatus umi_editor_wb_multi_cursor_model_add(UmiEditorWbMultiCursorModel *m,UmiEditorWbPosition p){size_t i;if(m==NULL||p.line==0U||p.column==0U)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<m->count;i++)if(umi_editor_wb_position_compare(m->cursors[i],p)==0)return UMI_STATUS_ALREADY_EXISTS;if(m->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;m->cursors[m->count++]=p;return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_multi_cursor_model_remove(UmiEditorWbMultiCursorModel *m,UmiEditorWbPosition p){size_t i;if(m==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<m->count;i++)if(umi_editor_wb_position_compare(m->cursors[i],p)==0){if(i+1U<m->count)memmove(&m->cursors[i],&m->cursors[i+1U],(m->count-i-1U)*sizeof m->cursors[0]);m->count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
