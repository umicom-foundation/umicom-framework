/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/filter_rule.h
 *
 * PURPOSE:
 *   Filter context delivery by schema and typed field values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_FILTER_RULE_H
#define UMICOM_CONTEXT_CHANNEL_FILTER_RULE_H
#include "umicom/context_channel/payload.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_FILTER_RULE_MAX_ITEMS 128U
/**
 * Represent the context filter rule data shared with callers of this public contract.
 */
typedef struct UmiContextFilterRule {
    uint32_t structure_size;
    char filter_id[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char field_name[UMI_CONTEXT_TEXT_CAPACITY];
    char expected_text[UMI_CONTEXT_VALUE_CAPACITY];
    bool invert;
    bool enabled;
    uint64_t revision;
} UmiContextFilterRule;
/**
 * Represent the context filter rule store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextFilterRuleStore {
    UmiContextFilterRule items[UMI_CONTEXT_FILTER_RULE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextFilterRuleStore;
/**
 * Initialise context filter rule from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_filter_rule_init(UmiContextFilterRule *record);
/**
 * Check that context filter rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_filter_rule_validate(const UmiContextFilterRule *record);
/**
 * Initialise context filter rule store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_filter_rule_store_init(UmiContextFilterRuleStore *store);
/**
 * Provide the context filter rule store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_filter_rule_store_put(UmiContextFilterRuleStore *store,const UmiContextFilterRule *record);
/**
 * Remove context filter rule store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_filter_rule_store_remove(UmiContextFilterRuleStore *store,const char *identity);
/**
 * Find context filter rule store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextFilterRule *umi_context_filter_rule_store_find(UmiContextFilterRuleStore *store,const char *identity);
/**
 * Provide the context filter rule store find const operation used by this module and its
 * client applications.
 */
const UmiContextFilterRule *umi_context_filter_rule_store_find_const(const UmiContextFilterRuleStore *store,const char *identity);
/**
 * Return the number of records represented by context filter rule store without changing
 * their state.
 */
size_t umi_context_filter_rule_store_count(const UmiContextFilterRuleStore *store);
/**
 * Provide the context filter rule store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_filter_rule_store_snapshot(const UmiContextFilterRuleStore *store,UmiContextFilterRule *out_records,size_t capacity,size_t *out_count);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_filter_rule_archive_encode(const UmiContextFilterRule *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_filter_rule_archive_decode(const void *bytes, size_t byte_count,
    UmiContextFilterRule *value);

#ifdef __cplusplus
}
#endif
#endif
