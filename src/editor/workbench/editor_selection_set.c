/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_selection_set.c
 *
 * PURPOSE:
 *   Maintain a bounded set of editor selections with one primary selection.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_selection_set.h"

#include <string.h>
void umi_editor_wb_editor_selection_set_init(UmiEditorWbEditorSelectionSet *s){if(s!=NULL)memset(s,0,sizeof *s);}
UmiStatus umi_editor_wb_editor_selection_set_add(UmiEditorWbEditorSelectionSet *s,UmiEditorWbRange r,bool primary){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;if(s->count>=UMI_EDITOR_WB_MAX_SEGMENTS)return UMI_STATUS_CAPACITY_EXCEEDED;r=umi_editor_wb_range_normalize(r);s->ranges[s->count]=r;if(primary)s->primary_index=s->count;s->count++;return UMI_STATUS_OK;}
void umi_editor_wb_editor_selection_set_clear(UmiEditorWbEditorSelectionSet *s){if(s!=NULL){s->count=0U;s->primary_index=0U;}}
