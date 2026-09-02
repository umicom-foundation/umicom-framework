/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_item.c
 *
 * PURPOSE:
 *   Describe one open editor item independently of any toolkit tab widget.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_item.h"

#include <string.h>
/*
 * Initialise editor wb editor item from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_editor_item_init(UmiEditorWbEditorItem *item,const char *id,const char *path,UmiEditorWbOpenMode mode){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL||!umi_editor_wb_id_valid(id)||path==NULL)return UMI_STATUS_INVALID_ARGUMENT; memset(item,0,sizeof *item); /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_editor_wb_copy_text(item->item_id,sizeof item->item_id,id)!=UMI_STATUS_OK||umi_editor_wb_copy_text(item->path,sizeof item->path,path)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED; item->open_mode=mode; item->pinned=(mode==UMI_EDITOR_WB_OPEN_PINNED); item->revision=1U; return UMI_STATUS_OK;}
/*
 * Provide the editor wb editor item set dirty operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_editor_item_set_dirty(UmiEditorWbEditorItem *item,bool dirty){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT; item->dirty=dirty; item->revision++; return UMI_STATUS_OK;}
/*
 * Check that editor wb editor item satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_editor_item_valid(const UmiEditorWbEditorItem *item){return item!=NULL&&umi_editor_wb_id_valid(item->item_id)&&item->path[0]!='\0'&&item->open_mode>=UMI_EDITOR_WB_OPEN_NORMAL&&item->open_mode<=UMI_EDITOR_WB_OPEN_PINNED;}
