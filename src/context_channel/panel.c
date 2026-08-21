/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/panel.c
 *
 * PURPOSE:
 *   Implement register reusable cross-application panel contributions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/panel.h"
#include <string.h>
void umi_context_panel_init(UmiCrossApplicationPanel *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_context_panel_validate(const UmiCrossApplicationPanel *record)
{
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->panel_id, sizeof(record->panel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->application_id, sizeof(record->application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->title, sizeof(record->title))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->component_id, sizeof(record->component_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->default_channel_id, sizeof(record->default_channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if(record->panel_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void umi_context_panel_store_init(UmiCrossApplicationPanelStore *store)
{
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiCrossApplicationPanel *umi_context_panel_store_find(UmiCrossApplicationPanelStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].panel_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiCrossApplicationPanel *umi_context_panel_store_find_const(const UmiCrossApplicationPanelStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].panel_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_context_panel_store_put(UmiCrossApplicationPanelStore *store,const UmiCrossApplicationPanel *record)
{
    UmiCrossApplicationPanel *existing;
uint64_t next_revision;
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_context_panel_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_panel_store_find(store,record->panel_id);
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    if(store->count>=UMI_CONTEXT_PANEL_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
UmiStatus umi_context_panel_store_remove(UmiCrossApplicationPanelStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i){
if(strcmp(store->items[i].panel_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
size_t umi_context_panel_store_count(const UmiCrossApplicationPanelStore *store){
return store==NULL?0U:store->count;
}
UmiStatus umi_context_panel_store_snapshot(const UmiCrossApplicationPanelStore *store,UmiCrossApplicationPanel *out_records,size_t capacity,size_t *out_count)
{
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
