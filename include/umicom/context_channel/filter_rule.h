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
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_FILTER_RULE_MAX_ITEMS 128U
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
typedef struct UmiContextFilterRuleStore {
    UmiContextFilterRule items[UMI_CONTEXT_FILTER_RULE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextFilterRuleStore;
void umi_context_filter_rule_init(UmiContextFilterRule *record);
UmiStatus umi_context_filter_rule_validate(const UmiContextFilterRule *record);
void umi_context_filter_rule_store_init(UmiContextFilterRuleStore *store);
UmiStatus umi_context_filter_rule_store_put(UmiContextFilterRuleStore *store,const UmiContextFilterRule *record);
UmiStatus umi_context_filter_rule_store_remove(UmiContextFilterRuleStore *store,const char *identity);
UmiContextFilterRule *umi_context_filter_rule_store_find(UmiContextFilterRuleStore *store,const char *identity);
const UmiContextFilterRule *umi_context_filter_rule_store_find_const(const UmiContextFilterRuleStore *store,const char *identity);
size_t umi_context_filter_rule_store_count(const UmiContextFilterRuleStore *store);
UmiStatus umi_context_filter_rule_store_snapshot(const UmiContextFilterRuleStore *store,UmiContextFilterRule *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
