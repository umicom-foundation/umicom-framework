/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_selection_set.c
 *
 * PURPOSE:
 *   Maintain a bounded set of editor selections with one primary selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_selection_set.h"

#include <string.h>
/*
 * Initialise editor wb editor selection set from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_editor_selection_set_init(UmiEditorWbEditorSelectionSet *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Add editor wb editor selection set only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_wb_editor_selection_set_add(UmiEditorWbEditorSelectionSet *s,UmiEditorWbRange r,bool primary){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;r=umi_editor_wb_range_normalize(r);s->ranges[s->count]=r;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(primary)s->primary_index=s->count;s->count++;return UMI_STATUS_OK;}
/*
 * Release or reset state held by editor wb editor selection set so the same storage can be
 * reused safely.
 */
void umi_editor_wb_editor_selection_set_clear(UmiEditorWbEditorSelectionSet *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL){s->count=0U;s->primary_index=0U;}}
