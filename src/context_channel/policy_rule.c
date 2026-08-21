/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/policy_rule.c
 *
 * PURPOSE:
 *   Implement express data-sharing policy at the context boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/policy_rule.h"
#include <string.h>
void umi_context_policy_rule_init(UmiContextPolicyRule *record)
{
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
UmiStatus umi_context_policy_rule_validate(const UmiContextPolicyRule *record)
{
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->rule_id, sizeof(record->rule_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->schema_id, sizeof(record->schema_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->source_application_id, sizeof(record->source_application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if (!umi_context_text_is_valid(record->target_application_id, sizeof(record->target_application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    if(record->rule_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
void umi_context_policy_rule_store_init(UmiContextPolicyRuleStore *store)
{
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
UmiContextPolicyRule *umi_context_policy_rule_store_find(UmiContextPolicyRuleStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].rule_id,identity)==0)return &store->items[i];
return NULL;
}
const UmiContextPolicyRule *umi_context_policy_rule_store_find_const(const UmiContextPolicyRuleStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return NULL;
for(i=0U;i<store->count;++i)if(strcmp(store->items[i].rule_id,identity)==0)return &store->items[i];
return NULL;
}
UmiStatus umi_context_policy_rule_store_put(UmiContextPolicyRuleStore *store,const UmiContextPolicyRule *record)
{
    UmiContextPolicyRule *existing;
uint64_t next_revision;
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    if(umi_context_policy_rule_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_policy_rule_store_find(store,record->rule_id);
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    if(store->count>=UMI_CONTEXT_POLICY_RULE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
UmiStatus umi_context_policy_rule_store_remove(UmiContextPolicyRuleStore *store,const char *identity)
{
size_t i;
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
for(i=0U;i<store->count;++i){
if(strcmp(store->items[i].rule_id,identity)==0){
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
size_t umi_context_policy_rule_store_count(const UmiContextPolicyRuleStore *store){
return store==NULL?0U:store->count;
}
UmiStatus umi_context_policy_rule_store_snapshot(const UmiContextPolicyRuleStore *store,UmiContextPolicyRule *out_records,size_t capacity,size_t *out_count)
{
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}
