/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/editor/workbench/editor_item_registry.c
 *
 * PURPOSE:
 *   Own a bounded registry of open editor items with stable identities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/editor/workbench/editor_item_registry.h"

#include <string.h>
void umi_editor_wb_editor_item_registry_init(UmiEditorWbEditorItemRegistry *r){if(r!=NULL)memset(r,0,sizeof *r);}
UmiStatus umi_editor_wb_editor_item_registry_upsert(UmiEditorWbEditorItemRegistry *r,const UmiEditorWbEditorItem *x){size_t i;if(r==NULL||!umi_editor_wb_editor_item_valid(x))return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<r->count;i++)if(strcmp(r->items[i].item_id,x->item_id)==0){r->items[i]=*x;r->revision++;return UMI_STATUS_OK;}if(r->count>=UMI_EDITOR_WB_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;r->items[r->count++]=*x;r->revision++;return UMI_STATUS_OK;}
const UmiEditorWbEditorItem *umi_editor_wb_editor_item_registry_find(const UmiEditorWbEditorItemRegistry *r,const char *id){size_t i;if(r==NULL||id==NULL)return NULL;for(i=0U;i<r->count;i++)if(strcmp(r->items[i].item_id,id)==0)return &r->items[i];return NULL;}
UmiStatus umi_editor_wb_editor_item_registry_remove(UmiEditorWbEditorItemRegistry *r,const char *id){size_t i;if(r==NULL||id==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<r->count;i++)if(strcmp(r->items[i].item_id,id)==0){if(i+1U<r->count)memmove(&r->items[i],&r->items[i+1U],(r->count-i-1U)*sizeof r->items[0]);r->count--;r->revision++;return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
