/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/policy_rule.h
 *
 * PURPOSE:
 *   Express data-sharing policy at the context boundary.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_POLICY_RULE_H
#define UMICOM_CONTEXT_CHANNEL_POLICY_RULE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_POLICY_RULE_MAX_ITEMS 128U
typedef struct UmiContextPolicyRule {
    uint32_t structure_size;
    char rule_id[UMI_CONTEXT_TEXT_CAPACITY];
    char schema_id[UMI_CONTEXT_TEXT_CAPACITY];
    char source_application_id[UMI_CONTEXT_TEXT_CAPACITY];
    char target_application_id[UMI_CONTEXT_TEXT_CAPACITY];
    UmiContextPolicyDecision decision;
    uint32_t priority;
    bool enabled;
    uint64_t revision;
} UmiContextPolicyRule;
typedef struct UmiContextPolicyRuleStore {
    UmiContextPolicyRule items[UMI_CONTEXT_POLICY_RULE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextPolicyRuleStore;
void umi_context_policy_rule_init(UmiContextPolicyRule *record);
UmiStatus umi_context_policy_rule_validate(const UmiContextPolicyRule *record);
void umi_context_policy_rule_store_init(UmiContextPolicyRuleStore *store);
UmiStatus umi_context_policy_rule_store_put(UmiContextPolicyRuleStore *store,const UmiContextPolicyRule *record);
UmiStatus umi_context_policy_rule_store_remove(UmiContextPolicyRuleStore *store,const char *identity);
UmiContextPolicyRule *umi_context_policy_rule_store_find(UmiContextPolicyRuleStore *store,const char *identity);
const UmiContextPolicyRule *umi_context_policy_rule_store_find_const(const UmiContextPolicyRuleStore *store,const char *identity);
size_t umi_context_policy_rule_store_count(const UmiContextPolicyRuleStore *store);
UmiStatus umi_context_policy_rule_store_snapshot(const UmiContextPolicyRuleStore *store,UmiContextPolicyRule *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
