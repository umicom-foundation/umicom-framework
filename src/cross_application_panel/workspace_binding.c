/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/workspace_binding.c
 *
 * PURPOSE:
 *   Implement cross-application panel workspace binding validation and storage.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/workspace_binding.h"
#include <string.h>
void umi_panel_workspace_binding_init(UmiPanelWorkspaceBinding *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_panel_workspace_binding_validate(const UmiPanelWorkspaceBinding *record)
{
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->binding_id,sizeof(record->binding_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->workspace_id,sizeof(record->workspace_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->layout_id,sizeof(record->layout_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->perspective_id,sizeof(record->perspective_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(record->binding_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
void umi_panel_workspace_binding_store_init(UmiPanelWorkspaceBindingStore *store){
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiPanelWorkspaceBinding *umi_panel_workspace_binding_store_find(UmiPanelWorkspaceBindingStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].binding_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiPanelWorkspaceBinding *umi_panel_workspace_binding_store_find_const(const UmiPanelWorkspaceBindingStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].binding_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_panel_workspace_binding_store_put(UmiPanelWorkspaceBindingStore *store,const UmiPanelWorkspaceBinding *record){
UmiPanelWorkspaceBinding *existing;
uint64_t next;
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_panel_workspace_binding_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_workspace_binding_store_find(store,record->binding_id);
if(existing!=NULL){
next=existing->revision+1U;
*existing=*record;
existing->revision=next;
store->revision+=1U;
return UMI_STATUS_OK;
}
if(store->count>=UMI_PANEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
UmiStatus umi_panel_workspace_binding_store_remove(UmiPanelWorkspaceBindingStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].binding_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_panel_workspace_binding_store_snapshot(const UmiPanelWorkspaceBindingStore *store,UmiPanelWorkspaceBinding *records,size_t capacity,size_t *out_count){
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
