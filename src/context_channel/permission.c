/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/permission.c
 *
 * PURPOSE:
 *   Implement represent user and workload permissions for context operations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/permission.h"
#include <string.h>
void umi_context_permission_init(UmiContextPermission *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_context_permission_validate(const UmiContextPermission *record)
{
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->permission_id, sizeof(record->permission_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->subject_id, sizeof(record->subject_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->channel_id, sizeof(record->channel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if(record->permission_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void umi_context_permission_store_init(UmiContextPermissionStore *store)
{
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiContextPermission *umi_context_permission_store_find(UmiContextPermissionStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].permission_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiContextPermission *umi_context_permission_store_find_const(const UmiContextPermissionStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].permission_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_context_permission_store_put(UmiContextPermissionStore *store,const UmiContextPermission *record)
{
    UmiContextPermission *existing;
uint64_t next_revision;
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_context_permission_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_permission_store_find(store,record->permission_id);
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    if(store->count>=UMI_CONTEXT_PERMISSION_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
UmiStatus umi_context_permission_store_remove(UmiContextPermissionStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i){
if(strcmp(store->items[i].permission_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
size_t umi_context_permission_store_count(const UmiContextPermissionStore *store){
return store==NULL?0U:store->count;
}
UmiStatus umi_context_permission_store_snapshot(const UmiContextPermissionStore *store,UmiContextPermission *out_records,size_t capacity,size_t *out_count)
{
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
