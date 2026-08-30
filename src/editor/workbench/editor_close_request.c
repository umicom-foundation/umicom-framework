/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_close_request.c
 *
 * PURPOSE:
 *   Represent a governed request to close an editor item.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_close_request.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_close_request_init(UmiEditorWbEditorCloseRequest *s,const char *resource,const char *group){if(s==NULL||resource==NULL||resource[0]=='\0'||!umi_editor_wb_id_valid(group))return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->resource,sizeof s->resource,resource)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->group_id,sizeof s->group_id,group)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->mode=UMI_EDITOR_WB_OPEN_NORMAL;return UMI_STATUS_OK;} int umi_editor_wb_editor_close_request_valid(const UmiEditorWbEditorCloseRequest *s){return s!=NULL&&s->resource[0]!='\0'&&umi_editor_wb_id_valid(s->group_id);}
