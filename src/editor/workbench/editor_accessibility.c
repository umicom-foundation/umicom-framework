/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_accessibility.c
 *
 * PURPOSE:
 *   Describe screen-reader and keyboard-access metadata for an editor surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_accessibility.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_accessibility_init(UmiEditorWbEditorAccessibility *s,const char *id,const char *text){if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_editor_accessibility_set_values(UmiEditorWbEditorAccessibility *s,uint64_t a,uint64_t b,bool e){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_editor_accessibility_valid(const UmiEditorWbEditorAccessibility *s){return s!=NULL&&umi_editor_wb_id_valid(s->id);}
