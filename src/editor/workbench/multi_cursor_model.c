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
/*
 * Initialise editor wb multi cursor model from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_multi_cursor_model_init(UmiEditorWbMultiCursorModel *m){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m!=NULL)memset(m,0,sizeof *m);}
/*
 * Add editor wb multi cursor model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_multi_cursor_model_add(UmiEditorWbMultiCursorModel *m,UmiEditorWbPosition p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL||p.line==0U||p.column==0U)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_position_compare(m->cursors[i],p)==0)return UMI_STATUS_ALREADY_EXISTS;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;m->cursors[m->count++]=p;return UMI_STATUS_OK;}
/*
 * Remove editor wb multi cursor model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_wb_multi_cursor_model_remove(UmiEditorWbMultiCursorModel *m,UmiEditorWbPosition p){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(m==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<m->count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_position_compare(m->cursors[i],p)==0){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i+1U<m->count)memmove(&m->cursors[i],&m->cursors[i+1U],(m->count-i-1U)*sizeof m->cursors[0]);m->count--;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
