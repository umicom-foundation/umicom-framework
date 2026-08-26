/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/transient_editor.c
 *
 * PURPOSE:
 *   Track ephemeral editor activity and whether it should remain retained.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/transient_editor.h"

#include <string.h>
UmiStatus umi_editor_wb_transient_editor_init(UmiEditorWbTransientEditor *s,const char *id,const char *text){if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_transient_editor_set_values(UmiEditorWbTransientEditor *s,uint64_t a,uint64_t b,bool e){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_transient_editor_valid(const UmiEditorWbTransientEditor *s){return s!=NULL&&umi_editor_wb_id_valid(s->id);}
