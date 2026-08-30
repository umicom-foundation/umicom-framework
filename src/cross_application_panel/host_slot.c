/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/host_slot.c
 *
 * PURPOSE:
 *   Implement cross-application panel host slot validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/host_slot.h"
#include <string.h>
void umi_panel_host_slot_init(UmiPanelHostSlot *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_panel_host_slot_validate(const UmiPanelHostSlot *record)
{
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->slot_id,sizeof(record->slot_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->container_id,sizeof(record->container_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(record->slot_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
void umi_panel_host_slot_store_init(UmiPanelHostSlotStore *store){
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiPanelHostSlot *umi_panel_host_slot_store_find(UmiPanelHostSlotStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].slot_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiPanelHostSlot *umi_panel_host_slot_store_find_const(const UmiPanelHostSlotStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].slot_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_panel_host_slot_store_put(UmiPanelHostSlotStore *store,const UmiPanelHostSlot *record){
UmiPanelHostSlot *existing;
uint64_t next;
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_panel_host_slot_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_host_slot_store_find(store,record->slot_id);
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
UmiStatus umi_panel_host_slot_store_remove(UmiPanelHostSlotStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].slot_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_panel_host_slot_store_snapshot(const UmiPanelHostSlotStore *store,UmiPanelHostSlot *records,size_t capacity,size_t *out_count){
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
