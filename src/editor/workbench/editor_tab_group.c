/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_tab_group.c
 *
 * PURPOSE:
 *   Manage ordered tabs and active-tab state inside one editor group.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_tab_group.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_tab_group_init(UmiEditorWbEditorTabGroup *s,const char *id,const char *parent){if(s==NULL||!umi_editor_wb_id_valid(id)||parent==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(s,0,sizeof *s);if(umi_editor_wb_copy_text(s->id,sizeof s->id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(s->parent_id,sizeof s->parent_id,parent)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED;s->revision=1U;return UMI_STATUS_OK;} UmiStatus umi_editor_wb_editor_tab_group_set_count(UmiEditorWbEditorTabGroup *s,size_t count,size_t active){if(s==NULL||(count>0U&&active>=count))return UMI_STATUS_INVALID_ARGUMENT;s->item_count=count;s->active_index=count==0U?0U:active;s->active=count>0U;s->revision++;return UMI_STATUS_OK;} int umi_editor_wb_editor_tab_group_valid(const UmiEditorWbEditorTabGroup *s){return s!=NULL&&umi_editor_wb_id_valid(s->id)&&s->revision>0U&&(s->item_count==0U||s->active_index<s->item_count);}
