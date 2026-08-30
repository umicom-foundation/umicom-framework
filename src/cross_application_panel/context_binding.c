/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/context_binding.c
 *
 * PURPOSE:
 *   Implement cross-application panel context binding validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/context_binding.h"
#include <string.h>
void umi_panel_context_binding_init(UmiPanelContextBinding *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_panel_context_binding_validate(const UmiPanelContextBinding *record)
{
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->binding_id,sizeof(record->binding_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->channel_id,sizeof(record->channel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->schema_id,sizeof(record->schema_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(record->binding_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
void umi_panel_context_binding_store_init(UmiPanelContextBindingStore *store){
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiPanelContextBinding *umi_panel_context_binding_store_find(UmiPanelContextBindingStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].binding_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiPanelContextBinding *umi_panel_context_binding_store_find_const(const UmiPanelContextBindingStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].binding_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_panel_context_binding_store_put(UmiPanelContextBindingStore *store,const UmiPanelContextBinding *record){
UmiPanelContextBinding *existing;
uint64_t next;
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_panel_context_binding_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_context_binding_store_find(store,record->binding_id);
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
UmiStatus umi_panel_context_binding_store_remove(UmiPanelContextBindingStore *store,const char *identity){
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
UmiStatus umi_panel_context_binding_store_snapshot(const UmiPanelContextBindingStore *store,UmiPanelContextBinding *records,size_t capacity,size_t *out_count){
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
