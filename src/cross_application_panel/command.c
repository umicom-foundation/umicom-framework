/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/cross_application_panel/command.c
 *
 * PURPOSE:
 *   Implement cross-application panel command validation and storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/cross_application_panel/command.h"
#include <string.h>
void umi_panel_command_init(UmiPanelCommand *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_panel_command_validate(const UmiPanelCommand *record)
{
if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->command_id,sizeof(record->command_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->panel_id,sizeof(record->panel_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->instance_id,sizeof(record->instance_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->action_id,sizeof(record->action_id)))return UMI_STATUS_INVALID_ARGUMENT;
    if(!umi_context_text_is_valid(record->argument,sizeof(record->argument)))return UMI_STATUS_INVALID_ARGUMENT;
    if(record->command_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
return UMI_STATUS_OK;
}
void umi_panel_command_store_init(UmiPanelCommandStore *store){
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiPanelCommand *umi_panel_command_store_find(UmiPanelCommandStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].command_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiPanelCommand *umi_panel_command_store_find_const(const UmiPanelCommandStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].command_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_panel_command_store_put(UmiPanelCommandStore *store,const UmiPanelCommand *record){
UmiPanelCommand *existing;
uint64_t next;
if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(umi_panel_command_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
existing=umi_panel_command_store_find(store,record->command_id);
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
UmiStatus umi_panel_command_store_remove(UmiPanelCommandStore *store,const char *identity){
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].command_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
return UMI_STATUS_NOT_FOUND;
}
UmiStatus umi_panel_command_store_snapshot(const UmiPanelCommandStore *store,UmiPanelCommand *records,size_t capacity,size_t *out_count){
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
