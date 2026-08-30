/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/delivery_receipt.c
 *
 * PURPOSE:
 *   Implement retain per-target context delivery evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/delivery_receipt.h"
#include <string.h>
void umi_context_delivery_receipt_init(UmiContextDeliveryReceipt *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_context_delivery_receipt_validate(const UmiContextDeliveryReceipt *record)
{
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->receipt_id, sizeof(record->receipt_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->context_id, sizeof(record->context_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->subscription_id, sizeof(record->subscription_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->application_id, sizeof(record->application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->panel_id, sizeof(record->panel_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if(record->receipt_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void umi_context_delivery_receipt_store_init(UmiContextDeliveryReceiptStore *store)
{
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiContextDeliveryReceipt *umi_context_delivery_receipt_store_find(UmiContextDeliveryReceiptStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].receipt_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiContextDeliveryReceipt *umi_context_delivery_receipt_store_find_const(const UmiContextDeliveryReceiptStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].receipt_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_context_delivery_receipt_store_put(UmiContextDeliveryReceiptStore *store,const UmiContextDeliveryReceipt *record)
{
    UmiContextDeliveryReceipt *existing;
uint64_t next_revision;
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_context_delivery_receipt_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_delivery_receipt_store_find(store,record->receipt_id);
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    if(store->count>=UMI_CONTEXT_DELIVERY_RECEIPT_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
UmiStatus umi_context_delivery_receipt_store_remove(UmiContextDeliveryReceiptStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i){
if(strcmp(store->items[i].receipt_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
size_t umi_context_delivery_receipt_store_count(const UmiContextDeliveryReceiptStore *store){
return store==NULL?0U:store->count;
}
UmiStatus umi_context_delivery_receipt_store_snapshot(const UmiContextDeliveryReceiptStore *store,UmiContextDeliveryReceipt *out_records,size_t capacity,size_t *out_count)
{
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
