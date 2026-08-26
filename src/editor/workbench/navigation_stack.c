/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/navigation_stack.c
 *
 * PURPOSE:
 *   Maintain back/forward source navigation state without replacing canonical navigation history.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/navigation_stack.h"

#include <string.h>
void umi_editor_wb_navigation_stack_init(UmiEditorWbNavigationStack *s){if(s!=NULL)memset(s,0,sizeof *s);}
UmiStatus umi_editor_wb_navigation_stack_push(UmiEditorWbNavigationStack *s,const UmiEditorWbEditorLocation *l){if(s==NULL||!umi_editor_wb_editor_location_valid(l))return UMI_STATUS_INVALID_ARGUMENT;if(s->count>0U&&s->cursor+1U<s->count)s->count=s->cursor+1U;if(s->count>=UMI_EDITOR_WB_MAX_ITEMS){memmove(&s->entries[0],&s->entries[1],(UMI_EDITOR_WB_MAX_ITEMS-1U)*sizeof s->entries[0]);s->count--;if(s->cursor>0U)s->cursor--;}s->entries[s->count++]=*l;s->cursor=s->count-1U;return UMI_STATUS_OK;}
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_back(UmiEditorWbNavigationStack *s){if(s==NULL||s->count==0U||s->cursor==0U)return NULL;s->cursor--;return &s->entries[s->cursor];}
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_forward(UmiEditorWbNavigationStack *s){if(s==NULL||s->count==0U||s->cursor+1U>=s->count)return NULL;s->cursor++;return &s->entries[s->cursor];}
