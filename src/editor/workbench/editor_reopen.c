/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_reopen.c
 *
 * PURPOSE:
 *   Build a reopen request from recently-closed editor evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_reopen.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_reopen_init(UmiEditorWbEditorReopen *s,const char *resource,const char *group){if(s==NULL||resource==NULL||resource[0]=='\0'||!umi_editor_wb_id_valid(group))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->resource,sizeof s->resource,resource)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->group_id,sizeof s->group_id,group)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->mode=UMI_EDITOR_WB_OPEN_NORMAL;return UMI_STATUS_OK;} int umi_editor_wb_editor_reopen_valid(const UmiEditorWbEditorReopen *s){return s!=NULL&&s->resource[0]!='\0'&&umi_editor_wb_id_valid(s->group_id);}
