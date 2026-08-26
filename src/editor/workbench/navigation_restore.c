/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/navigation_restore.c
 *
 * PURPOSE:
 *   Represent a validated restore plan for a prior editor location.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/navigation_restore.h"

#include <string.h>
UmiStatus umi_editor_wb_navigation_restore_init(UmiEditorWbNavigationRestore *s,const char *id,const char *text){if(s==NULL||!umi_editor_wb_id_valid(id)||text==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->text,sizeof s->text,text)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_navigation_restore_set_values(UmiEditorWbNavigationRestore *s,uint64_t a,uint64_t b,bool e){if(s==NULL)return UMI_STATUS_INVALID_ARGUMENT;s->primary=a;s->secondary=b;s->enabled=e;return UMI_STATUS_OK;} int umi_editor_wb_navigation_restore_valid(const UmiEditorWbNavigationRestore *s){return s!=NULL&&umi_editor_wb_id_valid(s->id);}
