/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/transformer_rule.h
 *
 * PURPOSE:
 *   Describe explicit context transformations between compatible schemas.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_TRANSFORMER_RULE_H
#define UMICOM_CONTEXT_CHANNEL_TRANSFORMER_RULE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_TRANSFORMER_RULE_MAX_ITEMS 128U
/**
 * Represent the context transformer rule data shared with callers of this public contract.
 */
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
/**
 * Represent the context transformer rule store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextTransformerRuleStore {
    UmiContextTransformerRule items[UMI_CONTEXT_TRANSFORMER_RULE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextTransformerRuleStore;
/**
 * Initialise context transformer rule from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_transformer_rule_init(UmiContextTransformerRule *record);
/**
 * Check that context transformer rule satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_transformer_rule_validate(const UmiContextTransformerRule *record);
/**
 * Initialise context transformer rule store from caller-provided values so later
 * operations receive a known state.
 */
void umi_context_transformer_rule_store_init(UmiContextTransformerRuleStore *store);
/**
 * Provide the context transformer rule store put operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transformer_rule_store_put(UmiContextTransformerRuleStore *store,const UmiContextTransformerRule *record);
/**
 * Remove context transformer rule store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_transformer_rule_store_remove(UmiContextTransformerRuleStore *store,const char *identity);
/**
 * Find context transformer rule store while leaving the underlying catalogue or model
 * owned by this module.
 */
UmiContextTransformerRule *umi_context_transformer_rule_store_find(UmiContextTransformerRuleStore *store,const char *identity);
/**
 * Provide the context transformer rule store find const operation used by this module and
 * its client applications.
 */
const UmiContextTransformerRule *umi_context_transformer_rule_store_find_const(const UmiContextTransformerRuleStore *store,const char *identity);
/**
 * Return the number of records represented by context transformer rule store without
 * changing their state.
 */
size_t umi_context_transformer_rule_store_count(const UmiContextTransformerRuleStore *store);
/**
 * Provide the context transformer rule store snapshot operation used by this module and
 * its client applications.
 */
UmiStatus umi_context_transformer_rule_store_snapshot(const UmiContextTransformerRuleStore *store,UmiContextTransformerRule *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
