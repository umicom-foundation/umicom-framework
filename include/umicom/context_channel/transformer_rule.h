/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/transformer_rule.h
 *
 * PURPOSE:
 *   Describe explicit context transformations between compatible schemas.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_TRANSFORMER_RULE_H
#define UMICOM_CONTEXT_CHANNEL_TRANSFORMER_RULE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_TRANSFORMER_RULE_MAX_ITEMS 128U
typedef struct UmiContextTransformerRule {
    uint32_t structure_size;
    char transformer_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char target_schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_field[UMI_CONTEXT_TEXT_CAPACITY];
    char target_field[UMI_CONTEXT_TEXT_CAPACITY];
    bool enabled;
    uint64_t revision;
} UmiContextTransformerRule;
typedef struct UmiContextTransformerRuleStore {
    UmiContextTransformerRule items[UMI_CONTEXT_TRANSFORMER_RULE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextTransformerRuleStore;
void umi_context_transformer_rule_init(UmiContextTransformerRule *record);
UmiStatus umi_context_transformer_rule_validate(const UmiContextTransformerRule *record);
void umi_context_transformer_rule_store_init(UmiContextTransformerRuleStore *store);
UmiStatus umi_context_transformer_rule_store_put(UmiContextTransformerRuleStore *store,const UmiContextTransformerRule *record);
UmiStatus umi_context_transformer_rule_store_remove(UmiContextTransformerRuleStore *store,const char *identity);
UmiContextTransformerRule *umi_context_transformer_rule_store_find(UmiContextTransformerRuleStore *store,const char *identity);
const UmiContextTransformerRule *umi_context_transformer_rule_store_find_const(const UmiContextTransformerRuleStore *store,const char *identity);
size_t umi_context_transformer_rule_store_count(const UmiContextTransformerRuleStore *store);
UmiStatus umi_context_transformer_rule_store_snapshot(const UmiContextTransformerRuleStore *store,UmiContextTransformerRule *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
