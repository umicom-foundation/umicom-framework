/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_item.c
 *
 * PURPOSE:
 *   Describe one open editor item independently of any toolkit tab widget.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_item.h"

#include <string.h>
UmiStatus umi_editor_wb_editor_item_init(UmiEditorWbEditorItem *item,const char *id,const char *path,UmiEditorWbOpenMode mode){ if(item==NULL||!umi_editor_wb_id_valid(id)||path==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); if(umi_editor_wb_copy_text(item->item_id,sizeof item->item_id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(item->path,sizeof item->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED; item->open_mode=mode; item->pinned=(mode==UMI_EDITOR_WB_OPEN_PINNED); item->revision=1U; return UMI_STATUS_OK;}
UmiStatus umi_editor_wb_editor_item_set_dirty(UmiEditorWbEditorItem *item,bool dirty){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; item->dirty=dirty; item->revision++; return UMI_STATUS_OK;}
int umi_editor_wb_editor_item_valid(const UmiEditorWbEditorItem *item){return item!=NULL&&umi_editor_wb_id_valid(item->item_id)&&item->path[0]!='\0'&&item->open_mode>=UMI_EDITOR_WB_OPEN_NORMAL&&item->open_mode<=UMI_EDITOR_WB_OPEN_PINNED;}
