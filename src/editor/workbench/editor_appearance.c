/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_appearance.c
 *
 * PURPOSE:
 *   Describe editor-specific appearance choices on top of Framework appearance semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_appearance.h"

#include <string.h>
/*
 * Initialise editor wb editor appearance from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_appearance_init(UmiEditorWbEditorAppearance *s,const char *id,const char *text){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_editor_appearance_set_values(UmiEditorWbEditorAppearance *s,uint64_t a,uint64_t b,bool e){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_editor_appearance_valid(const UmiEditorWbEditorAppearance *s){return s!=NULL&&umi_editor_wb_id_valid(s->id);}
