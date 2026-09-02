/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/policy_rule.h
 *
 * PURPOSE:
 *   Express data-sharing policy at the context boundary.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_POLICY_RULE_H
#define UMICOM_CONTEXT_CHANNEL_POLICY_RULE_H
#include "umicom/context_channel/payload.h"
#ifdef __cplusplus
extern "C" {
#endif
#define UMI_CONTEXT_POLICY_RULE_MAX_ITEMS 128U
/**
 * Represent the context policy rule data shared with callers of this public contract.
 */
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
/**
 * Represent the context policy rule store data shared with callers of this public
 * contract.
 */
typedef struct UmiContextPolicyRuleStore {
    UmiContextPolicyRule items[UMI_CONTEXT_POLICY_RULE_MAX_ITEMS];
    size_t count;
    uint64_t revision;
} UmiContextPolicyRuleStore;
/**
 * Initialise context policy rule from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_policy_rule_init(UmiContextPolicyRule *record);
/**
 * Check that context policy rule satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_policy_rule_validate(const UmiContextPolicyRule *record);
/**
 * Initialise context policy rule store from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_policy_rule_store_init(UmiContextPolicyRuleStore *store);
/**
 * Provide the context policy rule store put operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_policy_rule_store_put(UmiContextPolicyRuleStore *store,const UmiContextPolicyRule *record);
/**
 * Remove context policy rule store while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_context_policy_rule_store_remove(UmiContextPolicyRuleStore *store,const char *identity);
/**
 * Find context policy rule store while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiContextPolicyRule *umi_context_policy_rule_store_find(UmiContextPolicyRuleStore *store,const char *identity);
/**
 * Provide the context policy rule store find const operation used by this module and its
 * client applications.
 */
const UmiContextPolicyRule *umi_context_policy_rule_store_find_const(const UmiContextPolicyRuleStore *store,const char *identity);
/**
 * Return the number of records represented by context policy rule store without changing
 * their state.
 */
size_t umi_context_policy_rule_store_count(const UmiContextPolicyRuleStore *store);
/**
 * Provide the context policy rule store snapshot operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_policy_rule_store_snapshot(const UmiContextPolicyRuleStore *store,UmiContextPolicyRule *out_records,size_t capacity,size_t *out_count);
#ifdef __cplusplus
}
#endif
#endif
