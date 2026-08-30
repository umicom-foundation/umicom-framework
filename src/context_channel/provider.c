/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/provider.c
 *
 * PURPOSE:
 *   Implement register context producers and external context providers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/provider.h"
#include <string.h>
void umi_context_provider_init(UmiContextProvider *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_context_provider_validate(const UmiContextProvider *record)
{
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->provider_id, sizeof(record->provider_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->application_id, sizeof(record->application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->display_name, sizeof(record->display_name))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->capability_id, sizeof(record->capability_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if(record->provider_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void umi_context_provider_store_init(UmiContextProviderStore *store)
{
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiContextProvider *umi_context_provider_store_find(UmiContextProviderStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].provider_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiContextProvider *umi_context_provider_store_find_const(const UmiContextProviderStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].provider_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_context_provider_store_put(UmiContextProviderStore *store,const UmiContextProvider *record)
{
    UmiContextProvider *existing;
uint64_t next_revision;
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_context_provider_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_provider_store_find(store,record->provider_id);
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    if(store->count>=UMI_CONTEXT_PROVIDER_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
UmiStatus umi_context_provider_store_remove(UmiContextProviderStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i){
if(strcmp(store->items[i].provider_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
size_t umi_context_provider_store_count(const UmiContextProviderStore *store){
return store==NULL?0U:store->count;
}
UmiStatus umi_context_provider_store_snapshot(const UmiContextProviderStore *store,UmiContextProvider *out_records,size_t capacity,size_t *out_count)
{
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
