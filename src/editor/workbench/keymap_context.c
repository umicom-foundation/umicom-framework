/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/keymap_context.c
 *
 * PURPOSE:
 *   Represent a key chord and semantic editor context constraint.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/keymap_context.h"

#include <string.h>
/*
 * Initialise editor wb keymap context from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_keymap_context_init(UmiEditorWbKeymapContext *s,const char *id,const char *text){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_keymap_context_set_values(UmiEditorWbKeymapContext *s,uint64_t a,uint64_t b,bool e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_keymap_context_valid(const UmiEditorWbKeymapContext *s){return s!=NULL&&umi_editor_wb_id_valid(s->id);}
