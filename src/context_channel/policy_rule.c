/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/context_channel/policy_rule.c
 *
 * PURPOSE:
 *   Implement express data-sharing policy at the context boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/context_channel/policy_rule.h"
#include "../base/value_archive_internal.h"
#include <string.h>
/*
 * Initialise context policy rule from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_policy_rule_init(UmiContextPolicyRule *record)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(record==NULL)return;
memset(record,0,sizeof(*record));
record->structure_size=(uint32_t)sizeof(*record);
record->revision=1U;
}
/*
 * Check that context policy rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_policy_rule_validate(const UmiContextPolicyRule *record)
{
    /* Fixed-size fields may come from a plug-in or restored state. Check
     * every terminator before the domain rules use these strings. Add each
     * new text field here so malformed input never reaches an unbounded read. */
    if (record == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->rule_id, '\0', sizeof(record->rule_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->schema_id, '\0', sizeof(record->schema_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->source_application_id, '\0', sizeof(record->source_application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (memchr(record->target_application_id, '\0', sizeof(record->target_application_id)) == NULL) return UMI_STATUS_INVALID_ARGUMENT;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(record==NULL||record->structure_size!=sizeof(*record))return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->rule_id, sizeof(record->rule_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->schema_id, sizeof(record->schema_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->source_application_id, sizeof(record->source_application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (!umi_context_text_is_valid(record->target_application_id, sizeof(record->target_application_id))) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(record->rule_id[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;
    return UMI_STATUS_OK;
}
/*
 * Initialise context policy rule store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_policy_rule_store_init(UmiContextPolicyRuleStore *store)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL)return;
memset(store,0,sizeof(*store));
store->revision=1U;
}
/*
 * Find context policy rule store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextPolicyRule *umi_context_policy_rule_store_find(UmiContextPolicyRuleStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].rule_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context policy rule store find const operation used by this module and its
 * client applications.
 */
const UmiContextPolicyRule *umi_context_policy_rule_store_find_const(const UmiContextPolicyRuleStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return NULL;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i)/* Keep the operation inside its valid bounds before reading, writing or adding data. */ if(strcmp(store->items[i].rule_id,identity)==0)return &store->items[i];
return NULL;
}
/*
 * Provide the context policy rule store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_policy_rule_store_put(UmiContextPolicyRuleStore *store,const UmiContextPolicyRule *record)
{
    UmiContextPolicyRule *existing;
uint64_t next_revision;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(store==NULL||record==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_context_policy_rule_validate(record)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    existing=umi_context_policy_rule_store_find(store,record->rule_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(existing!=NULL){
next_revision=existing->revision+1U;
*existing=*record;
existing->revision=next_revision;
store->revision+=1U;
return UMI_STATUS_OK;
}
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if(store->count>=UMI_CONTEXT_POLICY_RULE_MAX_ITEMS)return UMI_STATUS_CAPACITY_EXCEEDED;
    store->items[store->count]=*record;
store->items[store->count].revision=1U;
store->count+=1U;
store->revision+=1U;
return UMI_STATUS_OK;
}
/*
 * Remove context policy rule store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_policy_rule_store_remove(UmiContextPolicyRuleStore *store,const char *identity)
{
size_t i;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||identity==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/* Visit each bounded item once so every record receives the same rule. */
for(i=0U;i<store->count;++i){
/* Use the stable identifier comparison to choose the matching record or policy. */
if(strcmp(store->items[i].rule_id,identity)==0){
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(i+1U<store->count)memmove(&store->items[i],&store->items[i+1U],(store->count-i-1U)*sizeof(store->items[0]));
store->count-=1U;
memset(&store->items[store->count],0,sizeof(store->items[0]));
store->revision+=1U;
return UMI_STATUS_OK;
}
}
return UMI_STATUS_NOT_FOUND;
}
/*
 * Return the number of records represented by context policy rule store without changing
 * their state.
 */
size_t umi_context_policy_rule_store_count(const UmiContextPolicyRuleStore *store){
return store==NULL?0U:store->count;
}
/*
 * Provide the context policy rule store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_policy_rule_store_snapshot(const UmiContextPolicyRuleStore *store,UmiContextPolicyRule *out_records,size_t capacity,size_t *out_count)
{
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store==NULL||out_count==NULL)return UMI_STATUS_INVALID_ARGUMENT;
/*
 * Protect caller-owned memory by checking that required state is available before it is
 * used.
 */
if(store->count>capacity||(store->count!=0U&&out_records==NULL))return UMI_STATUS_CAPACITY_EXCEEDED;
/* Keep the operation inside its valid bounds before reading, writing or adding data. */
if(store->count!=0U)memcpy(out_records,store->items,store->count*sizeof(store->items[0]));
*out_count=store->count;
return UMI_STATUS_OK;
}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiContextPolicyRuleArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x7baff9f2ddb19dca);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPolicyRule *)0)->rule_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPolicyRule *)0)->schema_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPolicyRule *)0)->source_application_id)) * UINT64_C(1099511628211);
    schema = (schema ^ (uint64_t)sizeof(((UmiContextPolicyRule *)0)->target_application_id)) * UINT64_C(1099511628211);
    return schema;
}
static size_t UmiContextPolicyRuleArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U + sizeof(((UmiContextPolicyRule *)0)->rule_id) - 1U +
        8U + sizeof(((UmiContextPolicyRule *)0)->schema_id) - 1U +
        8U + sizeof(((UmiContextPolicyRule *)0)->source_application_id) - 1U +
        8U + sizeof(((UmiContextPolicyRule *)0)->target_application_id) - 1U +
        8U +
        8U +
        8U +
        8U;
}
static void UmiContextPolicyRuleArchiveWrite(UmiArchiveWriter *writer, const UmiContextPolicyRule *value)
{
    UmiArchiveWriteText(writer, value->rule_id, sizeof(value->rule_id));
    UmiArchiveWriteText(writer, value->schema_id, sizeof(value->schema_id));
    UmiArchiveWriteText(writer, value->source_application_id, sizeof(value->source_application_id));
    UmiArchiveWriteText(writer, value->target_application_id, sizeof(value->target_application_id));
    UmiArchiveWriteSigned(writer, (int64_t)value->decision);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->priority);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->enabled);
    UmiArchiveWriteUnsigned(writer, (uint64_t)value->revision);
}
static void UmiContextPolicyRuleArchiveRead(UmiArchiveReader *reader, UmiContextPolicyRule *value)
{
    value->structure_size = (uint32_t)sizeof(*value);
    UmiArchiveReadText(reader, value->rule_id, sizeof(value->rule_id));
    UmiArchiveReadText(reader, value->schema_id, sizeof(value->schema_id));
    UmiArchiveReadText(reader, value->source_application_id, sizeof(value->source_application_id));
    UmiArchiveReadText(reader, value->target_application_id, sizeof(value->target_application_id));
    value->decision = (UmiContextPolicyDecision)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->priority = (uint32_t)UmiArchiveReadUnsigned(reader, UINT32_MAX);
    value->enabled = (bool)UmiArchiveReadUnsigned(reader, 1U);
    value->revision = (uint64_t)UmiArchiveReadUnsigned(reader, UINT64_MAX);
}
static UmiStatus UmiContextPolicyRuleArchiveValidate(const UmiContextPolicyRule *value)
{
    return umi_context_policy_rule_validate(value);
}
UMI_DEFINE_VALUE_ARCHIVE(umi_context_policy_rule_archive_encode, umi_context_policy_rule_archive_decode,
    UmiContextPolicyRule, UmiContextPolicyRuleArchiveSchema, UmiContextPolicyRuleArchiveBound, UmiContextPolicyRuleArchiveWrite, UmiContextPolicyRuleArchiveRead, UmiContextPolicyRuleArchiveValidate)
