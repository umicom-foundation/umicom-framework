/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/navigation_stack.c
 *
 * PURPOSE:
 *   Maintain back/forward source navigation state without replacing canonical navigation history.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/navigation_stack.h"

#include <string.h>
/*
 * Initialise editor wb navigation stack from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_navigation_stack_init(UmiEditorWbNavigationStack *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s!=NULL)memset(s,0,sizeof *s);}
/*
 * Provide the editor wb navigation stack push operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_navigation_stack_push(UmiEditorWbNavigationStack *s,const UmiEditorWbEditorLocation *l){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_editor_location_valid(l))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>0U&&s->cursor+1U<s->count)s->count=s->cursor+1U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->count>=UMI_EDITOR_WB_MAX_ITEMS){memmove(&s->entries[0],&s->entries[1],(UMI_EDITOR_WB_MAX_ITEMS-1U)*sizeof s->entries[0]);s->count--;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->cursor>0U)s->cursor--;}s->entries[s->count++]=*l;s->cursor=s->count-1U;return UMI_STATUS_OK;}
/*
 * Provide the editor wb navigation stack back operation used by this module and its client
 * applications.
 */
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_back(UmiEditorWbNavigationStack *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->count==0U||s->cursor==0U)return NULL;s->cursor--;return &s->entries[s->cursor];}
/*
 * Provide the editor wb navigation stack forward operation used by this module and its
 * client applications.
 */
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_forward(UmiEditorWbNavigationStack *s){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||s->count==0U||s->cursor+1U>=s->count)return NULL;s->cursor++;return &s->entries[s->cursor];}
